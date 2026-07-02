# ☀️ UnderTheWatt

[🇫🇷 Français](./README.md) | 🇬🇧 English

**The real danger of the sun isn't measured in UV index, but in watts per square meter.**

The UV index, widely communicated to the public, only reflects the ultraviolet component of solar radiation — relevant for skin and eye risk, but not for the thermal load received by the body. Global solar radiation can exceed 1000 W/m² in full summer, close to the solar constant at ground level, and that's the parameter that actually matters for heatstroke risk in direct exposure. It's the data used by professional heat-stress indices (WBGT) for outdoor work and sport — yet it remains absent from public communication.

UnderTheWatt offers two ways to access it, depending on your skills and equipment:

| | For whom | What it is |
|---|---|---|
| [`widget/`](./widget) | Everyone | Standalone web widget (HTML/JS), fetches real solar radiation via the Open-Meteo API, no installation |
| [`firmware-solar-sensor/`](./firmware-solar-sensor) | Makers/hobbyists | DIY electronic build (ESP32-C6 + ADS1115 + PV cell) to measure irradiance yourself, MQTT publishing |

Everyone finds something useful here: the widget gives immediate access to the information from public weather data, while the sensor enables genuine local measurement for those who want to go further — and also serves as a cross-reference to calibrate the sensor.

This figure is nothing abstract: every summer, children die in France, forgotten in vehicles parked in the sun — a mechanism directly linked to this same radiation intensity. See [`docs/analyse-habitacle-voiture.md`](./docs/analyse-habitacle-voiture.md) for context and a detailed calculation of the thermal power received by a vehicle in full sun (~4.5 kW, the equivalent of a domestic heat pump). *(Note: this technical document is currently only available in French.)*

## `widget/` — web widget

- Real-time gauge for solar radiation (W/m²), temperature, and UV index, with color-coded risk levels.
- **Data source: satellite observation by default** (DWD MTG / EUMETSAT depending on region, via the [Open-Meteo Satellite Radiation API](https://open-meteo.com/en/docs/satellite-radiation-api)), with automatic fallback to the standard weather model if satellite data is unavailable for the requested area or time. A SATELLITE/MODEL selector lets you switch manually between the two sources.
- Automatic geolocation or city search (Open-Meteo API, no key required).
- Bilingual FR/EN, with two built-in explainer panels: the difference between UV index and global radiation, and a concrete benchmark comparing solar intensity to a domestic infrared heater.
- No dependencies, embeddable as an `<iframe>` on any site (see `widget/README` below or the Integration section).

Open `widget/index.html` in a browser to test locally, or host it on GitHub Pages / any static server.

### Why favor the satellite source

Open-Meteo's standard forecast API provides a **modeled** value (hourly average from weather models such as ECMWF/DWD/GFS), not a direct observation — solar radiation is one of the variables most sensitive to modeling error, with discrepancies on the order of 20-30% reported in some cases. The satellite API, on the other hand, relies on a **real observation of the sky** from a geostationary satellite (2.5 to 5 km resolution, updated every 10 to 30 minutes depending on the region), so it's noticeably more faithful to the current moment. It isn't yet available for North America, though — hence the automatic fallback to the model in that case, and the manual selector to compare the two.

### A benchmark to understand the stakes

The widget highlights global radiation (W/m²) rather than the UV index because that's the parameter reflecting the actual thermal load received by the body. To give a concrete sense of scale: a common domestic infrared heating panel (~600 W over ~0.6 m² of radiating surface) delivers a power density on the order of 1000 W/m² at its surface — the same order of magnitude as solar radiation in full summer. An intensity we instinctively associate with the comfort of indoor heating, never with danger — yet received directly on skin outdoors, it's a comparable thermal load.

### Embedding on another page

```html
<iframe
  src="https://papymakers.com/underthewatt/widget/index.html"
  style="width:100%; max-width:420px; height:560px; border:none;"
  loading="lazy"
  title="Solar exposure index">
</iframe>
```

## `firmware-solar-sensor/` — DIY sensor

A complete build to measure real solar radiation yourself, without a weather station:

- Reference PV cell + shunt resistor + ADS1115 (differential reading) + ESP32-C6, MQTT publishing.
- Ready-to-use PlatformIO firmware (`platformio.ini`, `src/main.cpp`), calibration remotely adjustable via MQTT without reflashing.
- Estimated total cost, excluding an ESP32 already in stock: €10-15.
- Priced bill of materials, shunt sizing, wiring, calibration procedure cross-checked against the widget/Open-Meteo, and a full comparison of possible PV cells (Adafruit/Voltaic, ANYSOLAR/IXOLAR range, professional sensors): see [`hardware/`](./hardware). *(Note: these hardware documents are currently only available in French.)*

## Known limitations

- The widget's risk scale (thresholds at 300 / 600 / 850 / 1000 W/m²) is a simple heuristic, not a validated medical index like WBGT. It gives a sense of scale and intuition, not a diagnosis.
- The DIY sensor requires a one-off manual calibration (no professional reference equipment) — indicative accuracy, not metrological.
- A flat cell without a diffusing dome slightly underestimates irradiance at low sun angles (imperfect cosine response).

## Repository deployment

```bash
cd UnderTheWatt
git init
git add .
git commit -m "UnderTheWatt v1 — widget + DIY sensor"
git remote add origin git@github.com:Papymakers/UnderTheWatt.git
git push -u origin main
```

Enable GitHub Pages (`main` branch, `/widget` folder) to get a public URL ready to use directly as an `<iframe>`.

## License

MIT — see [LICENSE](./LICENSE).
