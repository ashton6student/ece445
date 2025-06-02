
# PCB Project Overview

This repository contains the design files for two PCBs: **ESP32** and **Hall-Effect Sensor**. Both projects were designed using KiCad and include schematic files, PCB layouts, backups, and Gerber files for manufacturing.

## Project Structure

The repository is structured as follows:

```
PCB Files/
├── ESP32/
│   ├── ESP32.kicad_pcb           # PCB layout file
│   ├── ESP32.kicad_sch           # Schematic file
│   ├── ESP32.kicad_pro           # KiCad project file
│   ├── gerbers/                  # Gerber files for PCB manufacturing
│   └── backups/                  # Backup versions of the project
├── hall-effect/
│   ├── hall-effect.kicad_pcb     # PCB layout file
│   ├── hall-effect.kicad_sch     # Schematic file
│   ├── hall-effect.kicad_pro     # KiCad project file
│   ├── gerbers/                  # Gerber files for PCB manufacturing
│   └── backups/                  # Backup versions of the project
└── __MACOSX/                     # System-generated files (can be ignored)
```

## ESP32 Project

The ESP32 PCB project is designed around the ESP32 microcontroller, featuring:

- BLE and Wi-Fi connectivity
- GPIO headers for peripherals
- Power management circuits

### Gerber Files

The Gerber files located in `ESP32/gerbers/` include:

- Copper layers (F_Cu, B_Cu)
- Silkscreen layers (F_Silkscreen, B_Silkscreen)
- Solder mask layers (F_Mask, B_Mask)
- Paste layers (F_Paste, B_Paste)
- Edge cuts for board outline
- Drill files (PTH, NPTH)

## Hall-Effect Project

This project is based on a Hall-effect sensor module, designed for magnetic field detection.

- Uses a Hall-effect sensor IC
- Signal conditioning circuitry
- Gerber Files

Similar structure to the ESP32 project with copper, silkscreen, mask, paste, edge cuts, and drill files.

## Backups

The `backups/` directories in each project folder contain automated KiCad backups, timestamped for version tracking.

## Usage

Open the project files using KiCad (version 6.x or later). To fabricate the PCBs, use the Gerber files located in the respective `gerbers/` folder.
