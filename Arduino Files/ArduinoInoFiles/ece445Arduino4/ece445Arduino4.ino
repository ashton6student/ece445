#include <Arduino.h>
#include <math.h>

const int NUM_SENSORS = 5;

#define LED_PIN 45

// Sensor pins
const int xPins[NUM_SENSORS] = {26, 0, 34, 36, 38};
const int yPins[NUM_SENSORS] = {27, 33, 35, 39, 37};

// Joystick pins
const int joystickXPin = 1;
const int joystickYPin = 2;

// Buttons
const int NUM_BUTTONS = 2;
const int buttonPins[NUM_BUTTONS] = {25, 32};
const unsigned long debounceDelay = 50;

float alpha = 0.3;
float threshold = 2.0;
float tolerance = 5.0;

struct SensorPair {
  int xPin;
  int yPin;
  float xMin, xMax, yMin, yMax;
  float xFiltered, yFiltered;
  float lower, upper;
  int pressCount;
  float w;
  float mapped;
};

SensorPair sensors[NUM_SENSORS];

struct Button {
  bool currentState;
  bool lastState;
  bool debouncedState;
  unsigned long lastDebounceTime;
};

Button buttons[NUM_BUTTONS];

// Override mode
bool useOverride = false;
const float offsets[NUM_SENSORS] = {0.0, 0.2, 0.4, 0.6, 0.8};
float overrideValues[NUM_SENSORS];
const unsigned long cycleDuration = 1000;
unsigned long startTime;

int activeIndex = 0;
unsigned long button1PressTime = 0;
bool button1Held = false;

// Joystick smoothing
float joyXFiltered = 0;
float joyYFiltered = 0;
float joystickAlpha = 0.3;
float joystickThreshold = 5.0;

void generateOverrideValues() {
  unsigned long elapsed = (millis() - startTime) % cycleDuration;
  float rampValue = (float)elapsed / cycleDuration;

  for (int i = 0; i < NUM_SENSORS; i++) {
    float offsetValue = rampValue + offsets[i];
    if (offsetValue > 1.0) offsetValue -= 1.0;
    overrideValues[i] = offsetValue;
  }
}

void handleButtonPress(int buttonIndex) {
  switch (buttonIndex) {
    case 0:
      if (sensors[0].pressCount == 0) {
        for (int i = 0; i < NUM_SENSORS; i++) {
          sensors[i].lower = sensors[i].w;
          sensors[i].pressCount = 1;
        }
//        Serial.println("Lower bounds set for all sensors.");
      } else if (sensors[0].pressCount == 1) {
        for (int i = 0; i < NUM_SENSORS; i++) {
          sensors[i].upper = sensors[i].w;
          sensors[i].pressCount = 0;
        }
//        Serial.println("Upper bounds set for all sensors.");
      }
      break;

    case 1:
      activeIndex = (activeIndex + 1) % NUM_SENSORS;
//      Serial.println("Active Index: " + String(activeIndex));
      break;
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
//  for (int i = 0; i < NUM_BUTTONS; i++) {
//    pinMode(buttonPins[i], INPUT);
//    buttons[i] = {LOW, LOW, LOW, 0};
//  }

  for (int i = 0; i < NUM_SENSORS; i++) {
    sensors[i].xPin = xPins[i];
    sensors[i].yPin = yPins[i];

    int xInit = analogRead(sensors[i].xPin);
    int yInit = analogRead(sensors[i].yPin);

    sensors[i].xMin = sensors[i].xMax = xInit;
    sensors[i].yMin = sensors[i].yMax = yInit;

    sensors[i].xFiltered = xInit;
    sensors[i].yFiltered = yInit;

    sensors[i].lower = 0;
    sensors[i].upper = 0;
    sensors[i].pressCount = 0;
    sensors[i].w = 0;
    sensors[i].mapped = 0;
  }

 joyXFiltered = analogRead(joystickXPin);
 joyYFiltered = analogRead(joystickYPin);

  startTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  // --- Button Debouncing ---
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].currentState = digitalRead(buttonPins[i]);

    if (buttons[i].currentState != buttons[i].lastState) {
      buttons[i].lastDebounceTime = currentTime;
    }

    if ((currentTime - buttons[i].lastDebounceTime) > debounceDelay) {
      if (buttons[i].currentState != buttons[i].debouncedState) {
        buttons[i].debouncedState = buttons[i].currentState;

        if (buttons[i].debouncedState == HIGH) {
          handleButtonPress(i);
          if (i == 1) {
            button1PressTime = currentTime;
            button1Held = true;
          }
        }
      }
    }

    if (i == 1 && button1Held && buttons[i].debouncedState == LOW) {
      if (currentTime - button1PressTime > 1000) {
        useOverride = !useOverride;
        Serial.println(useOverride ? "Override Mode: ON" : "Override Mode: OFF");
      }
      button1Held = false;
    }

    buttons[i].lastState = buttons[i].currentState;
  }

  // --- Sensor Processing ---
  for (int i = 0; i < NUM_SENSORS; i++) {
    SensorPair &s = sensors[i];

    float x = analogRead(s.xPin);
    float y = analogRead(s.yPin);

    float xNext = alpha * x + (1 - alpha) * s.xFiltered;
    float yNext = alpha * y + (1 - alpha) * s.yFiltered;

    if (abs(xNext - s.xFiltered) > threshold) s.xFiltered = xNext;
    if (abs(yNext - s.yFiltered) > threshold) s.yFiltered = yNext;

    if (s.xFiltered < s.xMin) s.xMin = s.xFiltered;
    if (s.xFiltered > s.xMax) s.xMax = s.xFiltered;
    if (s.yFiltered < s.yMin) s.yMin = s.yFiltered;
    if (s.yFiltered > s.yMax) s.yMax = s.yFiltered;

    float xMid = (s.xMax + s.xMin) / 2.0;
    float xHalf = (s.xMax - s.xMin) / 2.0;
    float xNorm = (s.xFiltered - xMid) / xHalf;

    float yMid = (s.yMax + s.yMin) / 2.0;
    float yHalf = (s.yMax - s.yMin) / 2.0;
    float yNorm = (s.yFiltered - yMid) / yHalf;

    s.w = atan2(yNorm, xNorm) * 180.0 / PI + 180.0;

    float range = fmod(s.upper - s.lower + 360.0, 360.0);
    float delta = fmod(s.w - s.lower + 360.0 + tolerance, 360.0);
    s.mapped = (delta > range) ? 1.0 : delta / range;
  }

  // --- Override ---
  generateOverrideValues();

  // --- Joystick Smoothing ---
 int rawJoyX = analogRead(joystickXPin);
 int rawJoyY = analogRead(joystickYPin);

 float joyXNext = joystickAlpha * rawJoyX + (1 - joystickAlpha) * joyXFiltered;
 float joyYNext = joystickAlpha * rawJoyY + (1 - joystickAlpha) * joyYFiltered;

 if (abs(joyXNext - joyXFiltered) > joystickThreshold) joyXFiltered = joyXNext;
 if (abs(joyYNext - joyYFiltered) > joystickThreshold) joyYFiltered = joyYNext;

 int joyX = (int)joyXFiltered;
 int joyY = (int)joyYFiltered;

  // --- Output for Unity ---
  // for (int i = 0; i < NUM_SENSORS; i++) {
  //   float val = useOverride ? overrideValues[i] : sensors[i].mapped;
  //   Serial.print(val, 3);
  //   if (i < NUM_SENSORS - 1) Serial.print(", ");
  // }

  for (int i = 0; i < NUM_SENSORS; i++) {
    float val = useOverride ? overrideValues[i] : sensors[i].mapped;

    //Avoid sending NaN
    if (isnan(val)) val = 0.0;

    Serial.print(val, 3);
    if (i < NUM_SENSORS - 1) Serial.print(", ");
  }

 Serial.print(" | ");
 Serial.print(joyX);
 Serial.print(" | ");
 Serial.print(joyY);
 Serial.print(" | ");
  Serial.print(useOverride ? "1" : "0");
  Serial.print(" | ");
  Serial.print(buttons[0].debouncedState ? "1" : "0");
  Serial.print(" | ");
  Serial.print(buttons[1].debouncedState ? "1" : "0");
  Serial.println(" | " + String(activeIndex));

  // SensorPair &s = sensors[activeIndex];
  // float range = fmod(s.upper - s.lower + 360.0, 360.0);
  // float delta = fmod(s.w - s.lower + 360.0 + tolerance, 360.0);

  // Serial.println("button 0: " + String(buttons[0].currentState) +
  //               " button 1: " + String(buttons[1].currentState) +
  //               " press count: " + String(s.pressCount) +
  //               " w: " + String(s.w, 1) +
  //               " lower: " + String(s.lower, 2) +
  //               " upper: " + String(s.upper, 2) +
  //               " range: " + String(range, 2) +
  //               " delta: " + String(delta, 2) +
  //               " mapped: " + String(s.mapped, 2));

  delay(20);
}