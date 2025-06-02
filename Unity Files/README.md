
# Unity VR Project

## Overview

This Unity VR project is built using the XR Interaction Toolkit (version 3.0.7) and showcases a hand interaction demo. It serves as a foundational project for VR environments with hand tracking, hand-based interaction, and gesture control.

## Features

- **XR Interaction Toolkit (3.0.7)**: Utilizes Unity's toolkit for VR interaction.
- **Hands Interaction Demo**: Demonstrates the use of hand tracking, poke gestures, and interaction affordances.
- **VR Hand Rig**: Configured hand tracking for left and right hands with visual affordances.
- **Prefab Assets**: Includes various prefabs for hand interaction visualization and UI elements.
- **Custom Scripts**:
  - TransformSync.cs: Synchronizes object transformations.
  - ToggleGameObject.cs: Manages visibility toggling of objects.
  - PokeBlendShapeAnimator.cs: Animates hand poke gestures.
  - CollisionLogger.cs: Logs collisions detected by objects.
  - SerialFloatReader.cs: Reads floating-point data from a serial device.

## Installation

1. Clone the project to your Unity projects directory.
2. Open the project in Unity (version 2021.3 or higher recommended).
3. Import the XR Interaction Toolkit package via Unity Package Manager.

## Usage

- Load the `HandsDemoScene` from the `Assets/Samples/XR Interaction Toolkit/3.0.7/Hands Interaction Demo` folder.
- Press Play to explore the VR environment and interact with objects using hand gestures.
- Connect the VR headset and controllers to enable hand tracking.

## Scripts

- **HandsOneEuroFilterPostProcessor.cs**: Implements a post-processing filter for hand data.
- **PokeGestureDetector.cs**: Detects poke gestures and triggers associated actions.
- **PinchPointFollow.cs**: Updates the position of objects based on pinch gestures.
- **ObjectResetPlane.cs**: Resets objects to their initial positions when dropped.

## Troubleshooting

- Ensure the correct version of the XR Interaction Toolkit is installed.
- Verify that the VR headset is properly connected and configured.

## License

This project follows the standard Unity Asset Store license for included assets.
