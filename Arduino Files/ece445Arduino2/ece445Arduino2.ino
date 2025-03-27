const int xPin = 25;  // Analog input
const int yPin = 26;  // Analog input
const int buttonPin = 27;  // You can change this if needed

const float DEADZONE_THRESHOLD = 400.0;  // Adjust for 12-bit ADC

float lowerBound = 0;
float upperBound = 0;
bool lowerSet = false;
bool upperSet = false;

int pressCount = 0;
bool lastButtonState = HIGH;

void setup() {
  Serial.begin(115200); // ESP32 likes faster serial speeds
  pinMode(buttonPin, INPUT_PULLUP);  // Button wired to GND
}

void loop() {
  // --- Read analog inputs (0 to 4095 on ESP32) ---
  float x = analogRead(xPin);
  float y = analogRead(yPin);

  // --- Dead zone check ---
  float magnitude = sqrt(x * x + y * y);
  // if (magnitude < DEADZONE_THRESHOLD) {
  //   Serial.println("Inside dead zone — ignoring.");
  //   delay(50);
  //   return;
  // }

  // --- Phase calculation ---
  float phase = atan2(y, x); // Radians

  // --- Button press handling ---
  bool currentButtonState = digitalRead(buttonPin);
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    pressCount++;

    if (pressCount == 1) {
      lowerBound = phase;
      lowerSet = true;
      Serial.println("Lower bound set.");
    } else if (pressCount == 2) {
      upperBound = phase;
      upperSet = true;
      Serial.println("Upper bound set.");
    }
  }
  lastButtonState = currentButtonState;

  // --- Mapping phase to [0, 1] ---
  if (lowerSet && upperSet) {
    float mappedValue;

    // Handle reversed bounds
    if (upperBound < lowerBound) {
      float temp = lowerBound;
      lowerBound = upperBound;
      upperBound = temp;
    }

    if (phase <= lowerBound) {
      mappedValue = 0.0;
    } else if (phase >= upperBound) {
      mappedValue = 1.0;
    } else {
      mappedValue = (phase - lowerBound) / (upperBound - lowerBound);
    }

    Serial.print("Mapped Value: ");
    Serial.println(mappedValue, 3);
  }

  delay(20);
}
