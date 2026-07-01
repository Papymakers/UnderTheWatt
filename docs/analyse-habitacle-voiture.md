# Pourquoi ce projet compte : quand le rayonnement solaire devient un vrai danger

## Le point de départ

Ce dépôt part d'un constat simple : l'indice UV, largement communiqué au public, ne mesure qu'une petite tranche du rayonnement solaire (les ultraviolets) — pertinente pour le risque cutané et oculaire, mais pas pour la charge thermique reçue par le corps ou par un environnement clos. Le rayonnement solaire global, lui, dépasse régulièrement 1000 W/m² en plein été, et c'est ce paramètre qui compte réellement pour le risque de coup de chaleur.

Ce document rassemble deux éléments pour rendre ce chiffre concret : un cas réel où cette intensité tue, et un calcul de ce qu'elle représente à l'échelle d'un objet du quotidien — une voiture garée en plein soleil.

## Un cas extrême et documenté : l'habitacle de voiture

À l'été 2026, plusieurs décès d'enfants oubliés dans des véhicules ont été rapportés en France pendant les épisodes de canicule, notamment dans le Vaucluse (juin 2026). Les autorités sanitaires (Santé publique France, Sécurité routière) rappellent chaque été des faits établis, encore mal connus du grand public :

- Un habitacle fermé, même garé à l'ombre, peut gagner 10 à 15°C en quelques dizaines de minutes du fait de l'effet de serre créé par les vitres.
- La montée en température est rapide dès le début : environ 7 à 8°C dès les dix premières minutes.
- Laisser une vitre entrouverte de quelques centimètres ne ralentit pas significativement l'accumulation de chaleur — une idée reçue répandue mais inexacte.
- Dans plus de la moitié des cas d'hyperthermie mortelle chez l'enfant, il ne s'agit pas d'un abandon volontaire : c'est un phénomène de mémoire documenté en médecine (rupture de routine), pas un défaut d'attention consciente.

Le mécanisme physique est un effet de serre localisé : le rayonnement solaire traverse le pare-brise (transparent à la lumière visible et au proche infrarouge), chauffe les surfaces intérieures sombres (tableau de bord, sièges), qui réémettent en infrarouge lointain — une longueur d'onde que le verre ne laisse plus ressortir. La chaleur s'accumule dans un volume clos sans échappatoire, ce qui aggrave nettement la situation par rapport à une exposition en plein air où l'air circule librement.

**Sources** : Sécurité routière, Santé publique France, reportages de presse française (juin 2026) sur les épisodes de canicule et les décès en Vaucluse.

## Combien de puissance un véhicule garé absorbe-t-il réellement ?

Pour donner un ordre de grandeur concret, voici un calcul simple, avec les hypothèses explicites (nécessaires pour que le chiffre garde un sens).

### Hypothèses

- Irradiance solaire : 1000 W/m² (plein soleil, ciel dégagé, proche du zénith)
- Coefficient d'absorption α selon la finition :
  - Peinture sombre : α ≈ 0,90-0,95
  - Peinture claire : α ≈ 0,35-0,45
- Verre automobile standard : transmission ≈ 75 % du rayonnement incident vers l'intérieur

### 1. Le toit seul

Surface d'un toit de berline/compacte moyenne : ~1,8 m².

```
P_incidente = 1000 W/m² × 1,8 m² = 1800 W

P_absorbée (toit sombre) ≈ 1800 × 0,92 ≈ 1650 W
P_absorbée (toit clair)  ≈ 1800 × 0,40 ≈  720 W
```

1650 W en continu correspond à un radiateur électrique domestique de forte puissance. Même en toit clair, 720 W reste comparable à une plaque de cuisson électrique.

Cette puissance chauffe la tôle du toit (jusqu'à 70-80°C en surface), qui se propage ensuite par conduction vers l'habitacle — mais ce n'est pas le mécanisme principal de l'emballement thermique : celui-ci vient surtout des vitres (voir ci-dessous).

### 2. Le véhicule entier (carrosserie + vitrage)

| Surface | Aire | Comportement |
|---|---|---|
| Toit | 1,8 m² | opaque, absorbe |
| Capot | 1,3 m² | opaque, absorbe |
| Coffre | 1,0 m² | opaque, absorbe |
| **Sous-total carrosserie** | **4,1 m²** | |
| Pare-brise + lunette + vitres latérales | 2,5 m² | transmet vers l'intérieur |

**Carrosserie opaque** (α moyen ≈ 0,65, mix teintes claires/foncées) :
```
P_carrosserie = 1000 W/m² × 4,1 m² × 0,65 ≈ 2665 W
```

**Vitrage** (transmission ≈ 75 %, énergie transmise presque intégralement absorbée à l'intérieur par des matériaux sombres — tableau de bord, sièges) :
```
P_transmise = 1000 W/m² × 2,5 m² × 0,75 ≈ 1875 W
```

**Total**
```
P_totale ≈ 2665 + 1875 ≈ 4540 W, soit environ 4,5 kW
```

Avec les marges d'incertitude sur les coefficients réels (peinture, tintage des vitres), on est raisonnablement dans une fourchette de **4 à 5 kW** en plein soleil, zénith, ciel dégagé.

### Mise en perspective

C'est le même ordre de grandeur qu'une pompe à chaleur mono-split de ~5 kW dimensionnée pour chauffer une maison individuelle en hiver. Le soleil déverse, en continu, sur une voiture garée, à peu près la puissance thermique nécessaire pour chauffer une maison entière — concentrée sur un habitacle clos de quelques mètres cubes, sans thermostat ni coupure.

C'est un calcul d'ordre de grandeur (surfaces et coefficients typiques, pas mesurés sur un véhicule réel), mais il donne une intuition physique directe de pourquoi un habitacle au soleil devient rapidement incompatible avec la vie — et pourquoi il vaut la peine de prendre au sérieux un chiffre comme "1000 W/m²" plutôt que de le laisser abstrait.
