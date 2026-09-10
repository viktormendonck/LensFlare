# Lensflare

Lensflare is a learning-focused RAW photo editor built in C++ with Qt 6 and QML.

This repository currently contains only the application scaffold. The goal is to keep the image-processing code separate from the UI so the core can remain usable without depending on QML.

`lensflare_core` is intended to contain the headless image-processing and project logic.

`lensflare` is the Qt Quick/QML application and should primarily be responsible for presentation and connecting the UI to the core.

## Dependencies

### Libraries

- Qt 6
  - Core
  - Gui
  - QML
  - Quick
  - Quick Controls
- LibRaw — RAW/CR2 decoding
- Exiv2 — image metadata
- LittleCMS 2 — color management
- nlohmann/json — JSON serialization for sidecar/project data

