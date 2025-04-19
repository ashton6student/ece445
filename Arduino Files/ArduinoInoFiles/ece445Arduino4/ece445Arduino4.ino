const int numValues = 5;
float values[numValues];

unsigned long startTime;
const unsigned long cycleDuration = 1000; // 1 second

// Small offsets for each value to stagger them slightly
const float offsets[numValues] = {0.0, 0.2, 0.4, 0.6, 0.8};

void setup() {
  Serial.begin(9600);
  startTime = millis();
}

void loop() {
  unsigned long elapsed = (millis() - startTime) % cycleDuration;
  float rampValue = (float)elapsed / cycleDuration; // 0.0 → 1.0 linearly

  // Apply the offset to each value
  for (int i = 0; i < numValues; i++) {
    float offsetValue = rampValue + offsets[i];
    if (offsetValue > 1.0) {
      offsetValue -= 1.0; // reset to 0 if it exceeds 1
    }
    values[i] = offsetValue;
  }

  // Print the values to Serial
  for (int i = 0; i < numValues; i++) {
    Serial.print(values[i], 3); // Print with 3 decimal places
    if (i < numValues - 1) Serial.print(", ");
  }
  Serial.println();

  delay(20); // ~50 updates per second
}
