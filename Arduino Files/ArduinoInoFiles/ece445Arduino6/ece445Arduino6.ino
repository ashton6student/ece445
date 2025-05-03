#include <Arduino.h>
#include <math.h>

// Led stuff
const int ledPin = 45;

bool ledValue = false;

// Sensor stuff
const int NUM_SENSORS = 5;

const int xPins[NUM_SENSORS] = {15, 6, 4, 10, 17};
const int yPins[NUM_SENSORS] = {16, 7, 5, 11, 18};

float alpha = 0.3;
float threshold = 2.0;
float tolerance = 5.0;
int pressCount = 0;

// Button stuff
const int NUM_BUTTONS = 1;

const int buttonPins[NUM_BUTTONS] = {12};

const unsigned long debounceDelay = 50;

// Joystick stuff
const int joyXPin = 14;
const int joyYPin = 13;

float joyXFiltered = 0;
float joyYFiltered = 0;
float joystickAlpha = 0.3;
float joystickThreshold = 5.0;
bool joystickFlag = false;

// Structs
struct SensorPair {
  int xPin;
  int yPin;
  float xMin, xMax, yMin, yMax;
  float xFiltered, yFiltered;
  float lower, upper;
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

// Functions
void handleButtonPress(int buttonIndex) {
  switch (buttonIndex) {
    case 0:
      if (pressCount == 0) {
        pressCount = 1;
        for (int i = 0; i < NUM_SENSORS; i++) {
          sensors[i].lower = sensors[i].w;
        }
      } else if (pressCount == 1) {
        pressCount = 2;
        for (int i = 0; i < NUM_SENSORS; i++) {
          sensors[i].upper = sensors[i].w;
        }
      }
      break;

    case 1:
      break;
  }
}

// Override stuff
bool useOverride = false;
const float offsets[NUM_SENSORS] = {0.0, 0.2, 0.4, 0.6, 0.8};
float overrideValues[NUM_SENSORS];
const unsigned long cycleDuration = 1000;
unsigned long startTime;

void generateOverrideValues() {
  unsigned long elapsed = (millis() - startTime) % cycleDuration;
  float rampValue = (float)elapsed / cycleDuration;

  for (int i = 0; i < NUM_SENSORS; i++) {
    float offsetValue = rampValue + offsets[i];
    if (offsetValue > 1.0) offsetValue -= 1.0;
    overrideValues[i] = offsetValue;
  }
}

void setup() {
  Serial.begin(115200);

  // Setup blink led
  pinMode(ledPin, OUTPUT);

  //Setup joystick
  pinMode(joyXPin, INPUT);
  pinMode(joyYPin, INPUT);
  
  //Setup buttons struct
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT);
    buttons[i] = {LOW, LOW, LOW, 0};
  }

  //Setup sensor struck
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
    sensors[i].w = 0;
    sensors[i].mapped = 0;
  }

  startTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  digitalWrite(ledPin, ledValue);
  ledValue = !ledValue;

  // --- Button Debouncing ---
  if (!joystickFlag) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
      buttons[i].currentState = digitalRead(buttonPins[i]);
      if (buttons[i].currentState != buttons[i].lastState) {
        buttons[i].lastDebounceTime = currentTime;
      }
      if ((currentTime - buttons[i].lastDebounceTime) > debounceDelay) {
        if (buttons[i].currentState != buttons[i].debouncedState) {
          buttons[i].debouncedState = buttons[i].currentState;
          if (buttons[i].debouncedState == LOW) {
            handleButtonPress(i);
          }
        }
      }
      buttons[i].lastState = buttons[i].currentState;
    }
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
  
      // --- Updated Mapping Logic ---
      const float adjustedTolerance = 25.0; // degrees of safe zone below lower
      float safeLower = fmod(s.lower - adjustedTolerance + 360.0, 360.0);
      float safeRange = fmod(s.upper - safeLower + 360.0, 360.0);
      float safeDelta = fmod(s.w - safeLower + 360.0, 360.0);
  
      s.mapped = (safeDelta > safeRange) ? 1.0 : safeDelta / safeRange;
  }


  if (useOverride) generateOverrideValues();

  int rawJoyX = analogRead(joyXPin);
  int rawJoyY = analogRead(joyYPin);
  float joyXNext = joystickAlpha * rawJoyX + (1 - joystickAlpha) * joyXFiltered;
  float joyYNext = joystickAlpha * rawJoyY + (1 - joystickAlpha) * joyYFiltered;
  if (abs(joyXNext - joyXFiltered) > joystickThreshold) joyXFiltered = joyXNext;
  if (abs(joyYNext - joyYFiltered) > joystickThreshold) joyYFiltered = joyYNext;

  for (int i = 0; i < NUM_SENSORS; i++) {
    float val = useOverride ? overrideValues[i] : sensors[i].mapped;
    if (isnan(val)) val = 0.0;
    Serial.print(val, 3);
    if (i < NUM_SENSORS - 1) Serial.print(",");
  }

  Serial.print(",");
  Serial.print(joyXFiltered, 2);
  Serial.print(",");
  Serial.print(joyYFiltered, 2);
  Serial.print(",");
//  Serial.print(pressCount);
//  Serial.print(",");
  Serial.println(buttons[0].debouncedState ? "0" : "1");
//  Serial.print(",");
//  Serial.println(buttons[1].debouncedState ? "1" : "0");

  delay(20);
}
