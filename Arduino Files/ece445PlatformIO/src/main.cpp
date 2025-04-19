#include <Arduino.h>
#include <math.h>

//define pins
const int xPin = 26;
const int yPin = 27;

//define global variables
float xMin, xMax, yMin, yMax;
float w;
bool lastButton = LOW;
int pressCount = 0;

float lower = 0;
float upper = 0;

float alpha = 0.3;
float threshold = 2;
float xfiltered = 0;
float yfiltered = 0;
float xnext = 0;
float ynext = 0;

//New button stuff
const int NUM_BUTTONS = 2;
const int buttonPins[NUM_BUTTONS] = {33, 25}; // Replace with your actual pins
const unsigned long debounceDelay = 50;

struct Button {
  bool currentState;
  bool lastState;
  bool debouncedState;
  unsigned long lastDebounceTime;
};

Button buttons[NUM_BUTTONS];

void handleButtonPress(int buttonIndex) {
  switch (buttonIndex) {
    case 0:
      // Example for button 0
      if (pressCount == 0) {
        lower = w;
        pressCount++;
      } else if (pressCount == 1) {
        upper = w;
        pressCount = 0;
      }
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT);
    buttons[i] = {LOW, LOW, LOW, 0};
  }
  //Set initial values
  xMin = analogRead(xPin);
  xMax = analogRead(xPin);
  yMin = analogRead(yPin);
  yMax = analogRead(yPin);
  xnext = analogRead(xPin);
  ynext = analogRead(yPin);
  xfiltered = analogRead(xPin);
  yfiltered = analogRead(yPin);
}

void loop() {
  //Get current readings
  float x = analogRead(xPin);
  float y = analogRead(yPin);
  xnext = alpha * x + (1 - alpha) * xfiltered;
  ynext = alpha * y + (1 - alpha) * yfiltered;

  if (abs(xnext - xfiltered) > threshold) {
    xfiltered = xnext;
  }

  if (abs(ynext - yfiltered) > threshold) {
    yfiltered = ynext;
  }
  //Update min/max's
  if (xfiltered < xMin) xMin = xfiltered;
  if (xfiltered > xMax) xMax = xfiltered;
  if (yfiltered < yMin) yMin = yfiltered;
  if (yfiltered > yMax) yMax = yfiltered;

  //Define and get local variables - Normalize x and y
  float xMid = (xMax + xMin) / 2.0;
  float xHalf = (xMax - xMin) / 2.0;
  float xNorm = (xfiltered - xMid) / xHalf;

  float yMid = (yMax + yMin) / 2.0;
  float yHalf = (yMax - yMin) / 2.0;
  float yNorm = (yfiltered - yMid) / yHalf;

  //Get angle, map to [0, 360)
  w = (atan2(yNorm, xNorm) * 180.0 / PI) + 180;  // w is in degrees, [-180, 180]

  //Button stuff
  unsigned long currentTime = millis();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].currentState = digitalRead(buttonPins[i]);

    if (buttons[i].currentState != buttons[i].lastState) {
      buttons[i].lastDebounceTime = currentTime;
    }

    if ((currentTime - buttons[i].lastDebounceTime) > debounceDelay) {
      if (buttons[i].currentState != buttons[i].debouncedState) {
        buttons[i].debouncedState = buttons[i].currentState;

        if (buttons[i].debouncedState == HIGH) {
          handleButtonPress(i);  // handle press based on index
        }
      }
    }

    buttons[i].lastState = buttons[i].currentState;
  }
  
  //Map (lower, upper) -> (0, 1)
  //First we need to shift our read in phase w such that w = 0 if it is lower. Assuming the delta(lower, upper) < 360, this will also fix the issue where lower > upper due to phase reset.
  float tolerance = 5.0;

  float range = fmod(upper - lower + 360.0, 360.0);
  float delta = fmod(w - lower + 360.0 + tolerance, 360.0);
  float hallf;

  float mapped;

  if (delta > range) {
    mapped = 1.0;
  } else {
    mapped = delta / range;
  }


  // Serial.println("x: " + String(x, 2) +
  //                " y: " + String(y, 2) +
  //                " filtered x: " + String(xfiltered, 2) +
  //                " filtered y: " + String(yfiltered, 2) +
  //                " w: " + String(w, 2));

  Serial.println("button 0: " + String(buttons[0].currentState) +
                 " button 1: " + String(buttons[1].currentState) +
                 " press count: " + String(pressCount) +
                 " w: " + String(w, 1) +
                 " lower: " + String(lower, 2) +
                 " upper: " + String(upper, 2) +
                 " range: " + String(range, 2) +
                 " delta: " + String(delta, 2) +                 
                 " mapped: " + String(mapped, 2));

  delay(20);
}

