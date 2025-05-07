# VR Gloves Project

Note: each subfolder has its own `README.md` file that forms the ECE 445 notebook, in addition to this top-level file.

## Overview

The VR Gloves project aims to create a pair of wearable hand-tracking gloves for VR applications, compatible with popular VR systems like Meta Quest 2. The gloves utilize perpendicular Hall effect sensors and magnets for finger flex tracking, a servo-based haptic feedback system, and BLE communication to interact with Unity applications.

## Inspirations

This project is inspired by LucidGloves, an open-source VR glove. This project specifically attempts to take the good things from V3 and the good things from V5 to make our own unique solution. Specifically, it uses the badge reel idea from V3 and the hall-effect magnet solution from the V5 and combines them together.

## Problem Statement

Modern VR devices often rely on handheld controllers that do not fully immerse the user in the virtual environment. While some high-end solutions like the Valve Index offer partial finger tracking, most affordable options still use conventional button-based input. The goal of this project is to develop a more immersive and affordable solution using DIY components.

## Key Features

- **Wireless Communication:** Uses BLE to transmit hand data to Unity applications.
- **Finger Tracking:** Perpendicular Hall effect sensors measure finger curl using magnetic field angle changes.
- **Haptic Feedback:** Servo motors lock the badge reels, restricting finger movement to provide haptic feedback.
- **Integrated VR Interaction:** Uses Unity's XR Interaction Toolkit to allow for these gloves to be developed in a VR scene and can be deployed to a VR headset.

## System Design

The VR gloves consist of five main subsystems:

### 1. Power Subsystem

- Uses a 7.4V Li-ion battery stepped down to 5V and 3.3V using LDO regulators.
- Provides stable power for ESP32-S3, hall effect sensors, and servo motors.

### 2. Sensor Subsystem

- Perpendicular Hall effect sensors paired with magnets on each finger track the degree of bend.
- Quest Controller used for hand positioning.
- Haptic feedback provided by servos locking badge reels.

### 3. MCU Subsystem

- Uses ESP32-S3 for data acquisition, BLE communication, and haptic control.
- Converts raw sensor data into a usable format in Unity.
- Transmits data via BLE to Unity with low latency (<20ms).

### 4. Controller Subsystem

- Features a thumb joystick and push buttons mounted on the index finger.
- Allows for conventional VR controller actions.
- Data is transmitted to Unity using the BLE protocol.

### 5. Software Subsystem

- Integrates with Unity using the XR Interaction Toolkit for virtual scene deployment.
- Virtual scene containing objects with colliders to test interaction.
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

### Arduino Files and Bluetooth Files

- Contains Arduino sketches for ESP32 to handle BLE communication and haptic control.
- Implements data acquisition from Hall effects and joystick inputs.
- Supports BLE wireless communication for real-time data transfer.

### Unity Files

- Unity scripts for receiving BLE data and controlling virtual hands.
- Includes a test scene for verifying glove performance.

### 3D Files

- 3D files for the glove parts.

### PCB Files

- KiCAD files for the PCBs in our project.

## Installation

1. Flash the ESP32 with the provided firmware in the `firmware/glove_controller/` directory.
2. Run the BLE Python bridge script located in `Bluetooth Files/`. You may need com2com software to simulate serial ports. Be sure to activate the Python virtual environment in `myenv/` and install required packages.
3. Open the Unity project from the `Unity Files/` folder.
4. Run the scene to test the glove inputs.

## Usage

- Wear the gloves and ensure they are powered on.
- Start the Unity scene and observe hand movement tracking.
- Test grabbing and manipulating virtual objects to validate haptic feedback.

## Future Improvements

- Enhanced haptic feedback.
- Integration with more VR platforms like SteamVR.
- Improved latency optimization in BLE communication.

## Authors

- Ashton Billings
- Hamza Lutfi
- Aditya Nebhrajani
