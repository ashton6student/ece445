# VR Gloves Project

## Overview

The VR Gloves project aims to create a pair of wearable hand-tracking gloves for VR applications, compatible with popular VR systems like Meta Quest 2. The gloves utilize potentiometers for finger flex tracking, a servo-based haptic feedback system, and BLE communication to interact with Unity applications. This project takes inspiration from UDCAP Kickstarter gloves and LucidGloves, enhancing them with a more affordable and DIY-friendly approach.

## Problem Statement

Modern VR devices often rely on handheld controllers that do not fully immerse the user in the virtual environment. While some high-end solutions like the Valve Index offer partial finger tracking, most affordable options still use conventional button-based input. The goal of this project is to develop a more immersive and affordable solution using DIY components.

## Key Features

- **Wireless Communication:** Uses BLE to transmit hand data to Unity applications.
- **Finger Tracking:** Potentiometers measure finger curl, enabling fine-grained control in VR environments.
- **Haptic Feedback:** Badge reels controlled by servo motors simulate force feedback when grasping virtual objects.
- **Integrated VR Interaction:** Uses Unity's XR Hands package to map glove input to virtual hands.
- **Customizable and Open Source:** Easy to replicate and customize.

## System Design

The VR gloves consist of five main subsystems:

### 1. Power Subsystem

- Uses a 7.4V Li-ion battery stepped down to 5V and 3.3V using LDO regulators.
- Provides stable power for ESP32-S3, potentiometers, and servo motors.
- Battery management ensures safe charging and power stability.

### 2. Sensor Subsystem

- Potentiometers on each finger track the degree of bend.
- Quest Controller used for hand positioning.
- Haptic feedback provided by servos locking badge reels.

### 3. MCU Subsystem

- Uses ESP32-S3 for data acquisition, BLE communication, and haptic control.
- Samples finger curl data at 50Hz and IMU data at 100Hz.
- Transmits data via BLE to Unity with low latency (<20ms).

### 4. Controller Subsystem

- Features a thumb joystick and push buttons mounted on the index finger.
- Allows for conventional VR controller actions.
- Data is transmitted to Unity using the BLE protocol.

### 5. Software Subsystem

- Integrates with Unity using the XR Hands package.
- BLE data mapped to virtual hand movements and actions.
- Collision detection triggers haptic feedback via BLE.

## Code Structure

```
project_root/
├── Unity Files/       # Unity project and scenes
├── PCB Files/         # PCB schematics and board files
├── Arduino Files/     # Arduino sketches for glove firmware
├── 3D Files/          # 3D models and assets for Unity
├── Bluetooth Files/   # BLE communication scripts and libraries
├── myenv/             # Virtual environment for Python utilities
└── README.md          # Project documentation
```

### Firmware

- Contains Arduino sketches for ESP32 to handle BLE communication and haptic control.
- Implements data acquisition from potentiometers and joystick inputs.
- Supports BLE wireless communication for real-time data transfer.

### Unity Integration

- Unity scripts for receiving BLE data and controlling virtual hands.
- Uses XR Hands package for realistic hand motion and interaction.
- Includes a test scene for verifying glove performance.

### Tools

- Utility Python scripts for data logging, testing, and BLE debugging.

### Assets

- 3D hand models and glove visualizations.
- Sample VR environment for testing hand interactions.

## Installation

1. Flash the ESP32 with the provided firmware in the `firmware/glove_controller/` directory.
2. Open the Unity project from the `unity/` folder.
3. Run the scene to test the glove inputs.

## Usage

- Wear the gloves and ensure they are powered on.
- Pair the gloves via BLE with the VR system.
- Start the Unity scene and observe hand movement tracking.
- Test grabbing and manipulating virtual objects to validate haptic feedback.

## Future Improvements

- Enhanced haptic feedback through force-sensitive resistors.
- Integration with more VR platforms like SteamVR.
- Improved latency optimization in BLE communication.

## Authors

- Ashton Billings
- Hamza Lutfi
- Aditya Nebhrajani
