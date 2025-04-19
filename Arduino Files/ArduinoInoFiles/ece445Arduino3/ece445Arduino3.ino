const int buttonPin = 25;
const int xPin = 26;
const int yPin = 27;

int xMin, xMax, yMin, yMax;
bool lastButtonState = HIGH;
int pressCount = 0;

float lowerBoundDeg = 0;
float upperBoundDeg = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP); // assuming button is wired to GND

  xMin = analogRead(xPin);
  xMax = analogRead(xPin);
  yMin = analogRead(yPin);
  yMax = analogRead(yPin);
}

void loop() {
  float x = analogRead(xPin);
  float y = analogRead(yPin);

  // Track min/max for normalization
  if (x < xMin) xMin = x;
  if (x > xMax) xMax = x;
  if (y < yMin) yMin = y;
  if (y > yMax) yMax = y;

  float xMid = (xMax + xMin) / 2.0;
  float xHalf = (xMax - xMin) / 2.0;
  float xNorm = (x - xMid) / xHalf;

  float yMid = (yMax + yMin) / 2.0;
  float yHalf = (yMax - yMin) / 2.0;
  float yNorm = (y - yMid) / yHalf;

  float w = atan2(yNorm, xNorm) * 180.0 / PI;  // w is in degrees, [-180, 180]

  Serial.println("x: " + String(xNorm, 2) +
                 " y: " + String(yNorm, 2) +
                 " w: " + String(w, 1));

  delay(20);
}
