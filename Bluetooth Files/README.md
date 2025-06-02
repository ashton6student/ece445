# Bluetooth Serial Bridge and ESP32 BLE API

## Overview

This folder consists of a Bluetooth serial bridge for communication between an ESP32 device and a host machine, along with an ESP32 Bluetooth API for setting up BLE communication. The folder is divided into two main components:

1. **Bluetooth Serial Bridge (Python)** - Establishes a Bluetooth connection between the ESP32 and a host PC.
2. **ESP32 Bluetooth API (Arduino)** - Sets up a BLE server on the ESP32 to facilitate communication with the host PC.

## Folder Structure

```
Bluetooth Files/
├── bluetooth_serial_bridge/
│   ├── bluetooth_serial_bridge.py        # Python script for Bluetooth serial bridge
│   └── test_bridge_legacy_app.py         # Placeholder for future integration with Unity's legacy COM code
└── esp32_bluetooth_api/
    └── bluetooth_api/
        └── bluetooth_api.ino             # Arduino sketch for BLE communication on ESP32
```

## Features

- Bidirectional Bluetooth communication using BLE.
- Virtual serial port support for both Windows and macOS.
- Real-time data transmission between the host and ESP32.

## Setup Instructions

### Prerequisites

- Python 3.8+
- `bleak` library for Bluetooth communication
- `pyserial` library for serial communication
- Arduino IDE for programming the ESP32

### Installing Dependencies

```bash
pip install bleak pyserial
```

### Running the Bridge

1. Upload the `bluetooth_api.ino` to your ESP32 using the Arduino IDE.
2. Run the Python bridge script:

```bash
python bluetooth_serial_bridge.py
```

3. Use the test script to send messages:

```bash
python test_bridge_legacy_app.py
```

## Usage

- The bridge script will automatically detect the OS and set up a virtual serial port.
- The test script sends user-inputted strings and displays responses from the ESP32.

## Troubleshooting

- Ensure that the COM port is correctly configured in the script for Windows.
- Check the BLE device name and UUIDs if connection fails.
