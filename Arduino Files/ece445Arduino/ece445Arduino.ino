#include <ESP32Servo.h>  // Use the ESP32 Servo library

// Button Pins
const int inputPin1 = 25;  // GPIO26 (Top Button - 'W')
const int inputPin2 = 26;  // GPIO25 (Bottom Button - 'S')

// Servo Pin
const int servoPin = 27;   // GPIO27 (Servo Signal Pin)

// Joystick Pins
const int VRX_PIN = 34;  // X-axis analog input (ADC1)
const int VRY_PIN = 35;  // Y-axis analog input (ADC1)
const int SW_PIN = 13;   // Joystick button input (digital)

//Potentiometer Pin
const int Pot1 = 14;
// Servo Object
Servo myServo;

void setup() {
    Serial.begin(115200);         // Start serial communication
    pinMode(inputPin1, INPUT);    // Set GPIO26 as INPUT (Top Button)
    pinMode(inputPin2, INPUT);    // Set GPIO25 as INPUT (Bottom Button)
    pinMode(Pot1, INPUT);    // Set GPIO14 as INPUT (Bottom Button)
    pinMode(SW_PIN, INPUT_PULLUP); // Enable pull-up resistor for joystick button

    myServo.attach(servoPin, 500, 2500); // Attach servo with pulse width limits
    myServo.write(0);                   // Initialize servo to 0° (locked position)
}

void loop() {
    // Read button states
    int topButton = digitalRead(inputPin1);     // Read 'W' button
    int bottomButton = digitalRead(inputPin2);  // Read 'S' button

    // Read joystick values
    int xValue = analogRead(VRX_PIN); // Read X-axis
    int yValue = analogRead(VRY_PIN); // Read Y-axis
    int buttonState = digitalRead(SW_PIN); // Read joystick button state


    //Read Pot
    int pot1 = analogRead(Pot1);

    // Servo control using buttons
    if (topButton == HIGH && bottomButton == LOW) {
        myServo.write(0);  // Rotate servo to 180° (spin forward)
    } 
    else if (bottomButton == HIGH && topButton == LOW) {
        myServo.write(110);   // Move servo to 0° (lock/stop)
    }

    // Print values to Serial Monitor
    Serial.print("w: ");
    Serial.print(topButton);
    Serial.print(" | s: ");
    Serial.print(bottomButton);
    Serial.print(" | 1: ");
    Serial.print(pot1);
    Serial.print(" | X: ");
    Serial.print(xValue);
    Serial.print(" | Y: ");
    Serial.print(yValue);
    Serial.print(" | Joystick Button: ");
    Serial.println(buttonState == LOW ? "Pressed" : "Released");

    delay(100);  // Debounce delay
}
