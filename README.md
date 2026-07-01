# ☀️ UnderTheWatt

**Le vrai risque du soleil ne se mesure pas en indice UV, mais en watts par mètre carré.**

L'indice UV, largement communiqué au public, ne reflète que la composante ultraviolette du rayonnement solaire — pertinente pour le risque cutané et oculaire, mais pas pour la charge thermique reçue par le corps. Le rayonnement solaire global peut dépasser 1000 W/m² en plein été, proche de la constante solaire au sol, et c'est ce paramètre qui compte réellement pour le risque de coup de chaleur en exposition directe. C'est la donnée utilisée par les indices de stress thermique professionnels (WBGT) pour le travail extérieur et le sport — mais elle reste absente de la communication grand public.

UnderTheWatt propose deux façons d'y accéder, selon vos compétences et votre matériel :

| | Pour qui | Ce que c'est |
|---|---|---|
| [`widget/`](./widget) | Tout le monde | Widget web autonome (HTML/JS), récupère le rayonnement solaire réel via l'API Open-Meteo, aucune installation |
| [`firmware-solar-sensor/`](./firmware-solar-sensor) | Makers/bricoleurs | Montage électronique DIY (ESP32-C6 + ADS1115 + cellule PV) pour mesurer soi-même l'irradiance, publication MQTT |

Chacun y trouve son compte : le widget donne accès à l'information immédiatement à partir de données météo publiques, le capteur permet une mesure locale réelle pour ceux qui veulent aller plus loin — et sert aussi de référence croisée pour étalonner le capteur.

Ce chiffre n'a rien d'abstrait : chaque été, des enfants meurent en France, oubliés dans des véhicules garés au soleil — un mécanisme directement lié à cette même intensité de rayonnement. Voir [`docs/analyse-habitacle-voiture.md`](./docs/analyse-habitacle-voiture.md) pour le contexte et un calcul détaillé de la puissance thermique reçue par un véhicule en plein soleil (~4,5 kW, l'équivalent d'une pompe à chaleur domestique).

## `widget/` — widget web

- Jauge en temps réel du rayonnement solaire (W/m²), température et indice UV, avec niveaux de risque codés couleur.
- **Source de données : observation satellite par défaut** (DWD MTG / EUMETSAT selon la zone, via l'[API Satellite Radiation d'Open-Meteo](https://open-meteo.com/en/docs/satellite-radiation-api)), avec repli automatique sur le modèle météo standard si la donnée satellite est indisponible pour la zone ou l'heure demandée. Un sélecteur SATELLITE/MODÈLE permet de basculer manuellement entre les deux sources.
- Géolocalisation automatique ou recherche de ville (API Open-Meteo, pas de clé requise).
- Bilingue FR/EN, deux blocs explicatifs intégrés : la différence entre indice UV et rayonnement global, et un repère concret comparant l'intensité solaire à un chauffage infrarouge domestique.
- Aucune dépendance, embarquable en `<iframe>` sur n'importe quel site (voir `widget/README` ci-dessous ou section Intégration).

Ouvrir `widget/index.html` dans un navigateur pour tester en local, ou héberger sur GitHub Pages / tout serveur statique.

### Pourquoi privilégier la source satellite

L'API de prévision standard d'Open-Meteo fournit une valeur **modélisée** (moyenne horaire issue de modèles météo type ECMWF/DWD/GFS), pas une observation directe — le rayonnement solaire est l'une des variables les plus sensibles aux erreurs de modélisation, avec des écarts de l'ordre de 20-30 % rapportés dans certains cas. L'API satellite, elle, s'appuie sur une **observation réelle du ciel** par satellite géostationnaire (résolution 2,5 à 5 km, mise à jour toutes les 10 à 30 minutes selon la zone), donc nettement plus fidèle à l'instant présent. Elle n'est toutefois pas encore disponible pour l'Amérique du Nord — d'où le repli automatique sur le modèle dans ce cas, et le sélecteur manuel pour comparer les deux.

### Un repère pour comprendre l'enjeu

Le widget met en avant le rayonnement global (W/m²) plutôt que l'indice UV parce que c'est ce paramètre qui reflète la charge thermique réelle reçue par le corps. Pour donner un ordre de grandeur concret : un panneau chauffant infrarouge domestique courant (~600 W pour ~0,6 m² de surface rayonnante) délivre une densité de puissance de l'ordre de 1000 W/m² à sa surface — le même ordre de grandeur que le rayonnement solaire en plein été. Une intensité que l'on associe spontanément au confort d'un chauffage d'intérieur, jamais à un danger — alors que reçue directement sur la peau en extérieur, c'est une charge thermique comparable.

### Intégration sur une autre page

```html
<iframe
  src="https://papymakers.com/underthewatt/widget/index.html"
  style="width:100%; max-width:420px; height:560px; border:none;"
  loading="lazy"
  title="Indice d'exposition solaire">
</iframe>
```

## `firmware-solar-sensor/` — capteur DIY

Montage complet pour mesurer soi-même le rayonnement solaire réel, sans station météo :

- Cellule PV de référence + shunt résistif + ADS1115 (lecture différentielle) + ESP32-C6, publication MQTT.
- Firmware PlatformIO prêt à l'emploi (`platformio.ini`, `src/main.cpp`), calibration ajustable à distance via MQTT sans reflash.
- Coût total estimé hors ESP32 déjà en stock : 10-15 €.
- Nomenclature chiffrée, dimensionnement du shunt, câblage, procédure d'étalonnage croisée avec le widget/Open-Meteo, et comparatif complet des cellules PV possibles (Adafruit/Voltaic, gamme ANYSOLAR/IXOLAR, capteurs professionnels) : voir [`hardware/`](./hardware).

## Limites connues

- L'échelle de risque du widget (seuils 300 / 600 / 850 / 1000 W/m²) est une heuristique simple, pas un indice médical validé type WBGT. Elle donne un ordre de grandeur et une intuition, pas un diagnostic.
- Le capteur DIY nécessite un étalonnage manuel ponctuel (pas de matériel de référence professionnel) — précision indicative, pas métrologique.
- Une cellule plate sans dôme diffuseur sous-estime légèrement l'irradiance aux faibles hauteurs de soleil (réponse cosinus imparfaite).

## Déploiement du dépôt

```bash
cd UnderTheWatt
git init
git add .
git commit -m "UnderTheWatt v1 — widget + capteur DIY"
git remote add origin git@github.com:Papymakers/UnderTheWatt.git
git push -u origin main
```

Activer GitHub Pages (branche `main`, dossier `/widget`) pour obtenir une URL publique directement utilisable en `<iframe>`.

## Licence

MIT — voir [LICENSE](./LICENSE).
