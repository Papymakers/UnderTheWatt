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
 *
 * Driver ADS1115 :
 *   Registres I2C natifs (Wire), sans dépendance à la librairie Adafruit
 *   ADS1X15 — même approche que le driver utilisé sur le projet de
 *   monitoring de courant triphasé. Conversion single-shot différentielle
 *   AIN0-AIN1, gain x16 (pleine échelle ±0.256V, 1 LSB = 7.8125µV).
 *   Registres et timing : datasheet Texas Instruments ADS1115 (SBAS444).
 *
 * Câblage I2C (XIAO ESP32-C6) :
 *   SDA → D4 (GPIO22)   SCL → D5 (GPIO23)
 *   ADS1115 ADDR relié à GND → adresse I2C 0x48 (par défaut).
 *
 * Diffusion temps réel / widget local :
 *   L'ESP32 héberge lui-même le widget (widget/index.html, copié dans
 *   data/index.html et déployé sur LittleFS) et diffuse chaque mesure
 *   à tous les clients connectés via une WebSocket sur /ws — c'est ce
 *   qui permet au widget d'afficher la mesure du capteur local en plus
 *   des sources SATELLITE/MODÈLE (Open-Meteo).
 *
 *   Ouvrir http://<IP_DU_CAPTEUR>/ dans un navigateur du même réseau
 *   local affiche directement le widget, connecté en direct au capteur.
 *   Le widget hébergé ailleurs (GitHub Pages, papymakers.com) peut aussi
 *   s'y connecter manuellement via le sélecteur de source "CAPTEUR" en
 *   saisissant l'IP du capteur (réseau local uniquement, pas de TLS).
 */

#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "config.h"

// ------------------------------------------------------------------
//  Driver ADS1115 natif — registres I2C directs (pas de lib Adafruit)
// ------------------------------------------------------------------
#define ADS1115_ADDR            0x48   // ADDR → GND
#define ADS1115_REG_CONVERSION  0x00
#define ADS1115_REG_CONFIG      0x01

#define ADS1115_OS_SINGLE       0x8000 // écriture : démarre une conversion / lecture : 1=terminée
#define ADS1115_MUX_DIFF_0_1    0x0000 // AIN0 - AIN1
#define ADS1115_PGA_16X         0x0E00 // ±0.256V, 1 LSB = 7.8125µV
#define ADS1115_MODE_SINGLE     0x0100 // single-shot
#define ADS1115_DR_128SPS       0x0080 // 128 éch/s — largement suffisant, mesure toutes les 15s
#define ADS1115_COMP_DISABLE    0x0003 // comparateur/ALERT désactivé

#define ADS1115_CONFIG_DIFF01 (ADS1115_OS_SINGLE | ADS1115_MUX_DIFF_0_1 | \
                                ADS1115_PGA_16X | ADS1115_MODE_SINGLE | \
                                ADS1115_DR_128SPS | ADS1115_COMP_DISABLE)

bool ads1115WriteConfig(uint16_t value) {
  Wire.beginTransmission(ADS1115_ADDR);
  Wire.write(ADS1115_REG_CONFIG);
  Wire.write((uint8_t)(value >> 8));
  Wire.write((uint8_t)(value & 0xFF));
  return Wire.endTransmission() == 0;
}

uint16_t ads1115ReadRegister(uint8_t reg) {
  Wire.beginTransmission(ADS1115_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return 0xFFFF;
  Wire.requestFrom((int)ADS1115_ADDR, 2);
  if (Wire.available() < 2) return 0xFFFF;
  uint16_t val = (uint16_t)Wire.read() << 8;
  val |= Wire.read();
  return val;
}

bool ads1115Present() {
  return ads1115ReadRegister(ADS1115_REG_CONFIG) != 0xFFFF;
}

// Lance une conversion single-shot différentielle AIN0-AIN1 et attend le
// résultat (bit OS repassé à 1 dans le registre config). Timeout 50ms —
// une conversion à 128SPS prend ~8ms, large marge de sécurité.
int16_t ads1115ReadDifferential01() {
  if (!ads1115WriteConfig(ADS1115_CONFIG_DIFF01)) return 0;

  unsigned long start = millis();
  uint16_t cfg;
  do {
    cfg = ads1115ReadRegister(ADS1115_REG_CONFIG);
  } while (!(cfg & ADS1115_OS_SINGLE) && millis() - start < 50);

  return (int16_t)ads1115ReadRegister(ADS1115_REG_CONVERSION);
}

// ------------------------------------------------------------------

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
Preferences prefs;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

float calibrationK = DEFAULT_CALIBRATION_K;
unsigned long lastPublish = 0;

// Construit le payload JSON partagé par MQTT et la WebSocket, à partir
// de la dernière mesure. Un seul format, deux canaux de diffusion.
size_t buildPayload(char* buffer, size_t bufferSize, float irradiance, float voltage_mV) {
  return snprintf(buffer, bufferSize,
    "{\"irradiance_wm2\":%.1f,\"raw_mV\":%.3f,\"calibration_k\":%.4f,\"rssi\":%d}",
    irradiance, voltage_mV, calibrationK, WiFi.RSSI());
}

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
    Serial.println("Widget local disponible sur http://" + WiFi.localIP().toString() + "/");
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

// Gestion minimale des événements WebSocket : on se contente de logguer
// les connexions/déconnexions, le capteur ne fait que diffuser (pas
// besoin de traiter les messages entrants des clients pour l'instant).
void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.print("WebSocket: client connecté #");
    Serial.println(client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.print("WebSocket: client déconnecté #");
    Serial.println(client->id());
  }
}

void setupWebServer() {
  if (!LittleFS.begin(true)) {
    Serial.println("Erreur : échec du montage LittleFS (widget local indisponible)");
  } else {
    Serial.println("LittleFS monté — widget local prêt à être servi.");
  }

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Sert le widget (data/index.html, uploadé via `pio run --target uploadfs`)
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.begin();
}

void setup() {
  Serial.begin(115200);
  delay(300);

  prefs.begin("solarSensor", false);
  calibrationK = prefs.getFloat("calib_k", DEFAULT_CALIBRATION_K);
  Serial.print("Calibration chargée : ");
  Serial.println(calibrationK, 4);

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  if (!ads1115Present()) {
    Serial.println("Erreur : ADS1115 non détecté (vérifier câblage I2C SDA/SCL et adresse 0x48)");
  }

  connectWiFi();
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(onMqttMessage);

  setupWebServer();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (!mqtt.connected()) {
    connectMqtt();
  }
  mqtt.loop();

  // Purge périodique des clients WebSocket déconnectés (recommandé par
  // ESPAsyncWebServer pour éviter l'accumulation de sockets fantômes).
  ws.cleanupClients();

  if (millis() - lastPublish >= PUBLISH_INTERVAL_MS) {
    lastPublish = millis();

    int16_t raw = ads1115ReadDifferential01();
    // Gain x16 : pleine échelle ±0.256V sur 16 bits signés → 1 LSB = 7.8125µV
    float voltage_mV = raw * 0.0078125f;
    if (voltage_mV < 0) voltage_mV = 0; // pas d'irradiance négative

    float irradiance = voltage_mV * calibrationK;

    char payload[192];
    buildPayload(payload, sizeof(payload), irradiance, voltage_mV);

    mqtt.publish(TOPIC_IRRADIANCE, payload);
    ws.textAll(payload);
    Serial.println(payload);
  }
}
