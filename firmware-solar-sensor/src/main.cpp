/*
 * UnderTheWatt — capteur de rayonnement solaire DIY (Papy Makers)
 * ESP32-C6 (XIAO) + ADS1115 + cellule PV de référence + shunt
 *
 * Principe :
 *   La cellule PV, quasi court-circuitée par le shunt, délivre un courant
 *   proportionnel à l'irradiance reçue (loi valable près du point Isc).
 *   L'ADS1115 lit la tension aux bornes du shunt en mode différentiel
 *   (AIN0-AIN1), avec un gain PGA élevé pour exploiter la faible tension
 *   de "burden" (quelques dizaines à centaines de mV).
 *
 *   irradiance_wm2 = tension_mV * calibration_k
 *
 *   calibration_k est déterminé empiriquement en comparant, un jour de
 *   ciel dégagé, la lecture brute à la valeur "shortwave_radiation"
 *   donnée par Open-Meteo pour votre position à cet instant précis.
 *   Voir la section "Étalonnage" du README.
 *
 * Le facteur de calibration peut être mis à jour à distance via MQTT
 * (topic retained), sans reflasher le firmware.
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_ADS1X15.h>
#include <Preferences.h>
#include "config.h"

Adafruit_ADS1115 ads;
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
Preferences prefs;

float calibrationK = DEFAULT_CALIBRATION_K;
unsigned long lastPublish = 0;

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("WiFi: connexion");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(400);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi OK, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi: échec de connexion, nouvelle tentative au prochain cycle");
  }
}

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  if (String(topic) == TOPIC_CALIBRATION) {
    float newK = msg.toFloat();
    if (newK > 0.0f) {
      calibrationK = newK;
      prefs.putFloat("calib_k", calibrationK);
      Serial.print("Calibration mise à jour via MQTT : ");
      Serial.println(calibrationK, 4);
    }
  }
}

void connectMqtt() {
  while (!mqtt.connected()) {
    Serial.print("MQTT: connexion...");
    bool ok;
    if (strlen(MQTT_USER) > 0) {
      ok = mqtt.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD);
    } else {
      ok = mqtt.connect(MQTT_CLIENT_ID);
    }
    if (ok) {
      Serial.println(" connecté.");
      mqtt.subscribe(TOPIC_CALIBRATION);
    } else {
      Serial.print(" échec, code=");
      Serial.print(mqtt.state());
      Serial.println(" — nouvelle tentative dans 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  prefs.begin("solarSensor", false);
  calibrationK = prefs.getFloat("calib_k", DEFAULT_CALIBRATION_K);
  Serial.print("Calibration chargée : ");
  Serial.println(calibrationK, 4);

  if (!ads.begin()) {
    Serial.println("Erreur : ADS1115 non détecté (vérifier câblage I2C SDA/SCL)");
  }
  // Gain élevé : plage utile ±256mV, résolution ~7.8µV/LSB.
  // À adapter si le burden voltage dépasse 256mV en plein soleil (réduire le shunt).
  ads.setGain(GAIN_SIXTEEN);

  connectWiFi();
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(onMqttMessage);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (!mqtt.connected()) {
    connectMqtt();
  }
  mqtt.loop();

  if (millis() - lastPublish >= PUBLISH_INTERVAL_MS) {
    lastPublish = millis();

    int16_t raw = ads.readADC_Differential_0_1();
    float voltage_mV = ads.computeVolts(raw) * 1000.0f;
    if (voltage_mV < 0) voltage_mV = 0; // pas d'irradiance négative

    float irradiance = voltage_mV * calibrationK;

    char payload[192];
    snprintf(payload, sizeof(payload),
      "{\"irradiance_wm2\":%.1f,\"raw_mV\":%.3f,\"calibration_k\":%.4f,\"rssi\":%d}",
      irradiance, voltage_mV, calibrationK, WiFi.RSSI());

    mqtt.publish(TOPIC_IRRADIANCE, payload);
    Serial.println(payload);
  }
}
