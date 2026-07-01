# Références de cellules PV — comparatif

Ce document recense toutes les références identifiées pour `PV1` lors des recherches, au-delà des deux options retenues dans `BOM-et-montage.md`. Utile si vous voulez comparer, dépanner un composant en rupture de stock, ou faire évoluer le montage.

## Recommandées dans ce dépôt

| Référence | Type | Specs clés | Prix | Où |
|---|---|---|---|---|
| **ANYSOLAR KXOB25-14X1F-TB** | SMD 8×23 mm | Voc=690 mV, Isc=58,6 mA, Pmpp=30,7 mW | ~2.90-3.15 € | [DigiKey](https://www.digikey.com/en/htmldatasheets/production/3648849/0/0/1/kxob25-14x1f-tr) · JLCPCB `C19270218` |
| **Adafruit #5855 / Voltaic P122** | Panneau ETFE 52×52 mm, 2 pastilles à souder | ~5,9 V à Vmpp, ~50 mA en plein soleil, IP67 | ~5,95 $ | [Fiche produit](https://www.adafruit.com/product/5855) |

Ce sont les deux références utilisées dans le calcul du shunt (`R_shunt`) documenté dans `BOM-et-montage.md`. Le stock fluctue chez tous les distributeurs — vérifiez la disponibilité au moment de la commande, ces références ont aussi des alternatives listées ci-dessous.

## Autres panneaux Adafruit / Voltaic (même gamme ETFE)

Toute la gamme Voltaic vendue par Adafruit utilise les mêmes cellules monocristallines SunPower (22+ % de rendement), IP67, construction ETFE. Seule la taille/puissance change. Les plus petits modèles sont les plus adaptés à un usage capteur (courant faible, shunt raisonnable) ; les plus grands sont pensés pour charger une batterie, pas pour servir de référence d'irradiance — retenus ici pour information.

| Réf. Voltaic | Produit Adafruit | Sortie | Connexion | Prix |
|---|---|---|---|---|
| P121 | [#5853](https://www.adafruit.com/product/5853) | ~2 V, ~150 mA peak (0,3 W) | 2 pastilles à souder | ~5,50 $ |
| **P122** | [#5855](https://www.adafruit.com/product/5855) | ~5,9 V, ~50 mA peak (0,3 W) | 2 pastilles à souder | ~5,95 $ |
| P123 | [#5856](https://www.adafruit.com/product/5856) | ~6 V, ~120 mA peak (0,6 W) | 2 pastilles à souder | ~8,95 $ |
| P124 | [#5368](https://www.adafruit.com/product/5368) | Voc=7,09 V, Vmpp=6,07 V, Impp=200 mA (1,2 W) | Jack DC 3,5/1,1 mm | ~14,95 $ |
| P126 | [#5366](https://www.adafruit.com/product/5366) | ~6 V, ~330 mA peak (2 W) | Jack DC 3,5/1,1 mm | ~20,95 $ |
| P105 | [#5367](https://www.adafruit.com/product/5367) | ~5 W | Jack DC | ~34,95 $ |
| P110 | [#5369](https://www.adafruit.com/product/5369) | ~10 W | Jack DC | ~64,95 $ |

Au-delà de P123 (0,6 W), le courant de court-circuit devient assez élevé pour nécessiter un shunt très faible (quelques dixièmes d'ohm) afin de rester dans la plage de l'ADS1115 — pas impossible, mais moins pratique qu'avec P121/P122/P123. Ces références sont donc plus adaptées à un usage "chargeur" qu'à un usage "capteur de référence".

## Références CMS ANYSOLAR / IXOLAR — état de disponibilité

La cellule SMD retenue (KXOB25-14X1F-TB) appartient à une famille plus large, dont certaines références historiquement citées dans des tutoriels sont aujourd'hui obsolètes :

| Référence | Statut | Remarque |
|---|---|---|
| **KXOB25-14X1F-TB** | ✅ Active, en stock | Retenue dans la BOM |
| KXOB25-12X1F-TB | ✅ Active | Variante proche, substituable |
| KXOB22-12X1F | ⚠️ Obsolète | Prédécesseur direct de la KXOB25-14X1F-TB, ne plus commander |
| SLMD121H04L (ex-IXYS) | ⚠️ Obsolète | Ancienne dénomination avant le rachat par ANYSOLAR/Littelfuse, citée dans d'anciens forums — à éviter |

## Capteurs professionnels préassemblés (hors dépôt, pour référence)

Si un jour vous voulez étalonner votre capteur DIY contre un instrument professionnel plutôt que contre le widget/Open-Meteo, voici les familles rencontrées pendant les recherches — hors budget d'un kit Papy Makers (plusieurs centaines d'euros), mais utiles comme repère de ce qui existe dans l'industrie :

- **Tritec Spektron 210/320/485mb** — capteur à cellule de référence, principe identique au nôtre (cellule silicium + shunt), calibré en usine par rapport au Fraunhofer ISE de Fribourg.
- **SEVEN Sensor** (capteurs Modbus/analogique) — même principe, avec compensation de température intégrée.
- **Tesup pyranomètre PV** — orienté toitures résidentielles/petites installations.

Ces produits confirment que le principe utilisé dans ce dépôt (cellule PV + shunt, near-short-circuit) est exactement celui de l'industrie du photovoltaïque pour ce type de mesure — la différence est uniquement le soin apporté à l'étalonnage et à la compensation de température.

## Notes de choix rapide

- **Vous voulez l'intégrer à un futur PCB EasyEDA/JLCPCB** → KXOB25-14X1F-TB.
- **Vous n'avez jamais soudé de CMS / vous voulez une première réalisation simple** → Adafruit #5855 (P122).
- **Vous préférez limiter les références à sourcer chez un seul distributeur** → tout du côté Adafruit (P121-P123 selon le courant souhaité), au prix d'un shunt légèrement différent à recalculer (formule dans `BOM-et-montage.md`).
