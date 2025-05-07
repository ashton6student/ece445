# Arduino Project README

## Project Overview

This project contains multiple Arduino-based applications designed to interface with sensors, buttons, and servos on ESP32 devices. The main focus is to control servo movements and read joystick inputs, with the data being transmitted via serial communication.

### Components:

- ESP32 microcontroller
- Joystick module
- Push buttons
- Servo motor
- Potentiometer

## Folder Structure

```
Arduino Files/
├── ece445PlatformIO/                # PlatformIO project for ESP32
│   ├── src/                         # Source files
│   │   └── main.cpp                 # Main application code
│   └── platformio.ini               # PlatformIO configuration file
├── ArduinoInoFiles/                 # Traditional Arduino sketches
│   ├── ece445Arduino/               # Main sketch folder
│   │   └── ece445Arduino.ino        # Primary Arduino sketch
│   ├── ece445Arduino2/              # Additional sketches
│   ├── ece445Arduino3/              # Additional sketches
│   ├── ece445Arduino4/              # Additional sketches
│   ├── ece445Arduino5/              # Additional sketches
│   └── ece445Arduino6/              # Additional sketches
```

## Installation

1. Clone or download the repository.
2. Open the PlatformIO project using VS Code or another IDE supporting PlatformIO.
3. Connect your ESP32 device to the USB port.
4. Upload the code using the following command:
   ```
   pio run --target upload
   ```

## Usage

The firmware sketches support both BLE (Bluetooth Low Energy) and wired serial communication, enabling wireless data transmission and control.

1. Upon startup, the ESP32 initializes the sensors and sets up the joystick and servo control.
2. Joystick data and button states are transmitted over serial at 115200 baud.
3. Use the joystick to control the servo and monitor sensor readings in the serial monitor.
4. BLE support can be integrated from `../Bluetooth Files`.

## Dependencies

- PlatformIO (for compilation and upload)
- Arduino framework for ESP32
- ESP32Servo library
