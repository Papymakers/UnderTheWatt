# Why this project matters: when solar radiation becomes a real danger

[🇫🇷 Français](./analyse-habitacle-voiture.md) | 🇬🇧 English

## The starting point

This repository starts from a simple observation: the UV index, widely communicated to the public, only measures a small slice of solar radiation (ultraviolet) — relevant for skin and eye risk, but not for the thermal load received by the body or by an enclosed space. Global solar radiation, on the other hand, regularly exceeds 1000 W/m² in full summer, and that's the parameter that actually matters for heatstroke risk.

This document brings together two elements to make that figure concrete: a real case where this intensity kills, and a calculation of what it represents at the scale of an everyday object — a car parked in full sun.

## An extreme, well-documented case: the car cabin

In summer 2026, several deaths of children forgotten in vehicles were reported in France during heatwave episodes, notably in the Vaucluse region (June 2026). Public health authorities (Santé publique France, Sécurité routière) remind the public every summer of established facts that remain poorly known:

- A closed cabin, even parked in the shade, can gain 10 to 15°C within a few dozen minutes due to the greenhouse effect created by the windows.
- The temperature rise is rapid from the very start: around 7 to 8°C within the first ten minutes.
- Leaving a window cracked open by a few centimeters does not significantly slow the heat buildup — a widespread but inaccurate belief.
- In more than half of fatal hyperthermia cases in children, it is not a case of deliberate abandonment: it's a documented memory phenomenon in medicine (disruption of routine), not a lapse of conscious attention.

The physical mechanism is a localized greenhouse effect: solar radiation passes through the windshield (transparent to visible light and near-infrared), heats dark interior surfaces (dashboard, seats), which re-emit in far infrared — a wavelength that glass no longer lets back out. Heat accumulates in an enclosed volume with no escape, which makes the situation markedly worse than outdoor exposure where air circulates freely.

**Sources**: Sécurité routière, Santé publique France, French press coverage (June 2026) of the heatwave episodes and the deaths in the Vaucluse.

## How much power does a parked vehicle actually absorb?

To give a concrete sense of scale, here is a simple calculation, with explicit assumptions (needed for the figure to remain meaningful).

### Assumptions

- Solar irradiance: 1000 W/m² (full sun, clear sky, near zenith)
- Absorption coefficient α depending on finish:
  - Dark paint: α ≈ 0.90-0.95
  - Light paint: α ≈ 0.35-0.45
- Standard automotive glass: transmission ≈ 75% of incident radiation to the interior

### 1. The roof alone

Roof area of an average sedan/compact car: ~1.8 m².

```
P_incident = 1000 W/m² × 1.8 m² = 1800 W

P_absorbed (dark roof)  ≈ 1800 × 0.92 ≈ 1650 W
P_absorbed (light roof) ≈ 1800 × 0.40 ≈  720 W
```

1650 W continuously corresponds to a high-power domestic electric heater. Even with a light-colored roof, 720 W remains comparable to an electric hot plate.

This power heats the roof sheet metal (up to 70-80°C at the surface), which then spreads by conduction into the cabin — but that's not the main driver of the thermal runaway: that mostly comes from the windows (see below).

### 2. The whole vehicle (body + glazing)

| Surface | Area | Behavior |
|---|---|---|
| Roof | 1.8 m² | opaque, absorbs |
| Hood | 1.3 m² | opaque, absorbs |
| Trunk | 1.0 m² | opaque, absorbs |
| **Body subtotal** | **4.1 m²** | |
| Windshield + rear window + side windows | 2.5 m² | transmits to interior |

**Opaque body** (average α ≈ 0.65, mix of light/dark finishes):
```
P_body = 1000 W/m² × 4.1 m² × 0.65 ≈ 2665 W
```

**Glazing** (transmission ≈ 75%, transmitted energy almost entirely absorbed inside by dark materials — dashboard, seats):
```
P_transmitted = 1000 W/m² × 2.5 m² × 0.75 ≈ 1875 W
```

**Total**
```
P_total ≈ 2665 + 1875 ≈ 4540 W, or roughly 4.5 kW
```

Given the uncertainty margins on the actual coefficients (paint, window tinting), this reasonably falls in a **4 to 5 kW** range in full sun, near zenith, clear sky.

### Putting it in perspective

That's the same order of magnitude as a ~5 kW mono-split heat pump sized to heat a single-family home in winter. The sun continuously pours onto a parked car roughly the thermal power needed to heat an entire house — concentrated into an enclosed cabin of a few cubic meters, with no thermostat and no cutoff.

This is an order-of-magnitude calculation (typical surfaces and coefficients, not measured on an actual vehicle), but it gives a direct physical intuition for why a car cabin in the sun quickly becomes incompatible with life — and why it's worth taking a figure like "1000 W/m²" seriously rather than leaving it abstract.
