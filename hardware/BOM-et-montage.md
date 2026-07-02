# Capteur de rayonnement solaire DIY — nomenclature et montage

## Principe

Une cellule photovoltaïque quasi court-circuitée par une faible résistance
shunt délivre un courant proportionnel à l'irradiance reçue (le courant de
court-circuit Isc d'une cellule PV est linéaire avec l'irradiance sur une
large plage — c'est le principe des cellules de référence utilisées dans
l'industrie photovoltaïque). L'ADS1115 lit en différentiel la tension aux
bornes du shunt, et l'ESP32-C6 convertit cette tension en W/m² via un
facteur de calibration déterminé empiriquement.

## Nomenclature (BOM)

| Réf. | Composant | Caractéristiques | Prix indicatif |
|---|---|---|---|
| U1 | XIAO ESP32-C6 | module MCU WiFi/BLE | ~6 € |
| U2 | ADS1115 | breakout ADC I2C 16 bits, 4 canaux | ~2 € |
| PV1 | **Option A — ANYSOLAR (ex-IXYS) KXOB25-14X1F-TB** | cellule PV monocristalline SMD, 8,0×23,0 mm, Voc=690 mV, Isc=58,6 mA, Vmpp=560 mV, Impp=55,0 mA, Pmpp=30,7 mW — [datasheet DigiKey](https://www.digikey.com/en/htmldatasheets/production/3648849/0/0/1/kxob25-14x1f-tr) — assemblable directement chez JLCPCB (réf. `C19270218`) | ~2.90-3.15 € |
| PV1 | **Option B — Adafruit #5855 / Voltaic P122** | mini panneau 10 cellules monocristallines SunPower, 52×52×3 mm, ~5.9 V à Vmpp, ~50 mA en plein soleil, 2 pastilles à souder (pas de CMS), déjà étanche IP67 — [fiche produit](https://www.adafruit.com/product/5855) | ~5.95 $ |
| R1 | Résistance shunt | 2.2 Ω, 1 %, 1/4 W avec PV1 option A · 2.7 Ω avec PV1 option B (voir dimensionnement) | ~0.1 € |
| — | Boîtier extérieur IP54/IP65 | avec presse-étoupe, fond blanc mat — nécessaire pour l'électronique ; superflu pour PV1 option B qui est déjà étanche | ~5 € |
| — | Connecteurs JST-PH 2 broches | raccordement cellule ↔ carte | ~0.5 € |
| — | Câble | 2 × 0.25 mm², longueur selon installation | — |

Total hors ESP32 déjà en stock : environ 10-15 €.

Deux profils d'utilisateurs, deux options pour PV1 :
- **Option A (SMD)** : conçue pour l'assemblage automatique chez JLCPCB, idéale si vous intégrez ce capteur à un futur PCB EasyEDA. Nécessite un minimum d'aisance en soudure CMS ou un assemblage industriel.
- **Option B (Adafruit/Voltaic)** : deux soudures classiques suffisent, déjà étanche, parfaite pour une première réalisation ou un public moins expérimenté en électronique.

Le firmware et la procédure d'étalonnage sont identiques quel que soit le choix — seul le facteur `calibration_k` (déterminé empiriquement, voir plus bas) diffère d'une cellule à l'autre.

## Dimensionnement du shunt

Objectif : rester proche des conditions de court-circuit (tension aux
bornes de la cellule la plus faible possible) tout en gardant une tension
lisible par l'ADS1115 avec une bonne résolution.

```
R_shunt = V_burden_max / Isc_max
```

Exemple avec PV1 option A (KXOB25-14X1F-TB, Isc = 58,6 mA à 1000 W/m², AM1.5) et une tension de burden visée à 130 mV (bien dans la plage ±256 mV du gain GAIN_SIXTEEN, avec marge pour des irradiances exceptionnelles au-delà de 1000 W/m²) :

```
R_shunt = 0.130 V / 0.0586 A ≈ 2.2 Ω
```

Avec PV1 option B (Adafruit/Voltaic P122, ~50 mA en plein soleil d'après la fiche produit), même raisonnement :

```
R_shunt = 0.130 V / 0.050 A ≈ 2.7 Ω
```

Ces deux valeurs sont retenues dans la nomenclature ci-dessus. Notez que le panneau Voltaic étant un assemblage de 10 cellules en série (jusqu'à ~5,9 V à vide), il reste correct de le faire fonctionner proche du court-circuit avec un shunt faible : c'est la tension aux bornes du shunt qui doit rester basse, pas la tension à vide du panneau.

Dans les deux cas, le courant indiqué par le fabricant est une valeur typique, pas garantie à l'unité — l'étalonnage empirique décrit plus bas (comparaison avec Open-Meteo) absorbe cet écart via le facteur `calibration_k`, donc un léger décalage sur R_shunt n'est pas bloquant.

## Câblage

```
Cellule PV (+)  ──────┬────────────────────  ADS1115 AIN0
                       │
                     [ R_shunt 2.2Ω ]
                       │
Cellule PV (−)  ──────┴────────────────────  ADS1115 AIN1

ADS1115  ── SDA ──  XIAO ESP32-C6 D4 (GPIO22)
ADS1115  ── SCL ──  XIAO ESP32-C6 D5 (GPIO23)
ADS1115  ── ADDR ── GND   (adresse I2C 0x48, valeur par défaut utilisée par le firmware)
ADS1115  ── VDD ──  3V3
ADS1115  ── GND ──  GND
```

La lecture différentielle AIN0−AIN1 élimine le besoin d'un amplificateur
opérationnel : l'ADS1115 mesure directement la faible tension aux bornes
du shunt avec une résolution de ~7.8 µV par LSB en gain ×16.

Le firmware pilote l'ADS1115 par accès direct aux registres I2C (voir
`src/main.cpp`), sans dépendance à une librairie tierce — même principe
que le driver utilisé sur le projet de monitoring de courant triphasé.
Si vous ajoutez un second ADS1115 sur le même bus pour un futur montage,
reliez son pin ADDR à VDD ou SDA/SCL pour lui donner une adresse I2C
différente (0x49/0x4A/0x4B) et évitez de câbler ADDR en l'air (flottant).

## Montage mécanique

- Cellule montée **à l'horizontale**, face vers le ciel dégagé, sans ombre portée (même partielle) à aucune heure de la journée.
- Éviter toute surface réfléchissante proche (mur clair, vitre) qui fausserait la lecture par réflexion parasite.
- Boîtier électronique séparé de la cellule si possible, relié par câble, pour limiter l'échauffement du boîtier par le soleil direct (dérive thermique de l'ADS1115 négligeable mais autant l'éviter).
- Une vitre ou un film de protection UV au-dessus de la cellule modifie sa réponse spectrale : à éviter, ou à inclure dans la calibration si présent en permanence.

**Limite connue** : une cellule plate sans dôme diffuseur sous-estime légèrement l'irradiance aux faibles hauteurs de soleil (réponse cosinus imparfaite, pertes de Fresnel en incidence rasante). Acceptable pour un indicateur de risque d'exposition à un instant donné ; à garder en tête si vous comparez à un pyranomètre professionnel.

## Étalonnage

1. Choisir un moment de ciel bien dégagé, si possible proche du zénith solaire (meilleure précision de la référence Open-Meteo).
2. Relever la valeur `raw_mV` publiée par le capteur (voir Serial ou topic MQTT `solarSensor/irradiance`).
3. Ouvrir le widget solaire ([`../widget/index.html`](../widget/index.html)) pour votre position, relever la valeur affichée en W/m² à ce même instant (donnée Open-Meteo `shortwave_radiation`).
4. Calculer :

```
calibration_k = irradiance_reference_wm2 / raw_mV
```

5. Publier cette valeur en MQTT retained sur le topic de calibration :

```bash
mosquitto_pub -h 192.168.1.20 -t solarSensor/config/calibration -m "9.47" -r
```

Le firmware l'enregistre en mémoire non volatile (Preferences) et l'applique immédiatement, sans reflash.

6. Idéalement, répéter la comparaison à 2-3 moments différents de la journée (matin, midi, fin d'après-midi) pour vérifier la cohérence et affiner `calibration_k` par une moyenne.

## Évolutions possibles

- Ajouter un second point de calibration à faible irradiance (aube/crépuscule) pour vérifier la linéarité sur toute la plage.
- Publier également la température ambiante (dérive thermique de Isc, faible mais mesurable, ~0.05 %/°C) pour une compensation fine.
- Version PCB dédiée (EasyEDA/JLCPCB) intégrant XIAO ESP32-C6, ADS1115 et connecteur cellule, dans l'esprit de vos autres cartes Papy Makers.
