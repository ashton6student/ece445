# Arduino Project README

## Project Overview

This folder contains various Arduino-based test sketches used throughout the development of the VR glove system. Each subfolder represents a different development or testing checkpoint. While several sketches were created for experimenting with sensor inputs, BLE, and motor control, the current active version is located in the `ece445Arduino5` folder.

### Components:

- ESP32 microcontroller
- Joystick module
- Push buttons
- Servo motor
- Potentiometer

### Development History

```
Arduino Files/
├── ece445PlatformIO/                # PlatformIO testing checkpoint for ESP32
│   ├── src/                         # Source files
│   │   └── main.cpp                 # Main application code
│   └── platformio.ini               # PlatformIO configuration file
├── ArduinoInoFiles/                 # Traditional Arduino sketches
│   ├── ece445Arduino/               # Initial sketch
│   │   └── ece445Arduino.ino        # Primary Arduino sketch
│   ├── ece445Arduino2/              # Development checkpoints
│   ├── ece445Arduino3/              # Development checkpoints
│   ├── ece445Arduino4/              # Development checkpoints
│   ├── ece445Arduino5/              # Current BLE version
│   └── ece445Arduino6/              # Current serial version
```

## Installation

1. Clone or download the repository.
2. Open the desired `.ino` file in the Arduino IDE:
   - Use `ece445Arduino5` for BLE communication.
   - Use `ece445Arduino6` for serial communication.
3. Connect your ESP32 board to your computer via USB.
4. In the Arduino IDE, select the correct board type and COM port.
5. Click the Upload button to flash the code to your ESP32.

## Usage

The firmware sketches support both BLE (Bluetooth Low Energy) and wired serial communication, enabling wireless data transmission and control.

1. Upon startup, the ESP32 initializes the sensors and sets up the joystick and servo control.
2. Joystick data and button states are transmitted over serial at 115200 baud.
3. Use the joystick to control the servo and monitor sensor readings in the serial monitor.
4. BLE support can be integrated from `../Bluetooth Files`.

## Dependencies

- Arduino framework for ESP32
- ESP32Servo library
