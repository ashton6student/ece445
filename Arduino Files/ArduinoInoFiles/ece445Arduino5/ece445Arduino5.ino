#include <Arduino.h>
#include <math.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_TX   "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RX   "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

// BLE stuff
BLECharacteristic* pTxCharacteristic;
bool deviceConnected = false;

#define RX_BUFFER_SIZE 256
char rxBuffer[RX_BUFFER_SIZE];
volatile int rxIndex = 0;
volatile bool rxLineReady = false;

#define TX_BUFFER_SIZE 256
char txBuffer[TX_BUFFER_SIZE];
int txIndex = 0;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) { deviceConnected = true; }
  void onDisconnect(BLEServer* pServer) { deviceConnected = false; }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    const uint8_t* data = pCharacteristic->getData();
    size_t len = pCharacteristic->getLength();
    for (size_t i = 0; i < len && rxIndex < RX_BUFFER_SIZE - 1; ++i) {
      char c = (char)data[i];
      if (c == '\n') {
        rxBuffer[rxIndex] = '\0';
        rxLineReady = true;
        rxIndex = 0;
        break;
      }
      rxBuffer[rxIndex++] = c;
    }
  }
};

void BLEUartBufferedSend(const char* msg) {
  if (!deviceConnected || !pTxCharacteristic) return;
  pTxCharacteristic->setValue((uint8_t*)msg, strlen(msg));
  pTxCharacteristic->notify();
}

int BLEUartReceive(char* buf, int maxLen) {
  if (rxLineReady) {
    rxLineReady = false;
    int len = strlcpy(buf, rxBuffer, maxLen);
    return len;
  }
  return 0;
}

// Hardware setup
const int NUM_SENSORS = 5;
const int xPins[NUM_SENSORS] = {15, 6, 4, 10, 17};
const int yPins[NUM_SENSORS] = {16, 7, 5, 11, 18};

const int ledPin = 45;
const int joyXPin = 14;
const int joyYPin = 13;
const int NUM_BUTTONS = 1;
const int buttonPins[NUM_BUTTONS] = {12};

float alpha = 0.3;
float threshold = 2.0;
float tolerance = 5.0;
const unsigned long debounceDelay = 50;

// Joystick smoothing
float joyXFiltered = 0;
float joyYFiltered = 0;
float joystickAlpha = 0.3;
float joystickThreshold = 5.0;

// Joystick calibration
float joyXCenter = 2048.0;
float joyYCenter = 2048.0;
float joyXRange = 2048.0;
float joyYRange = 2048.0;

// State
int pressCount = 0;
bool joystickFlag = false;
bool useOverride = false;

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

const float offsets[NUM_SENSORS] = {0.0, 0.2, 0.4, 0.6, 0.8};
float overrideValues[NUM_SENSORS];
const unsigned long cycleDuration = 1000;
unsigned long startTime;

void handleButtonPress(int buttonIndex) {
  if (buttonIndex == 0) {
    if (pressCount == 0) {
      pressCount = 1;
      for (int i = 0; i < NUM_SENSORS; i++) {
        sensors[i].lower = sensors[i].w;
      }
    } else if (pressCount == 1) {
      pressCount = 0;
      for (int i = 0; i < NUM_SENSORS; i++) {
        sensors[i].upper = sensors[i].w;
      }
    }
  }
}

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

  BLEDevice::init("ESP32-Finger-Sensors");
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_TX,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_RX,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();
  pServer->getAdvertising()->start();

  Serial.println("BLE UART Ready");

  pinMode(ledPin, OUTPUT);
  pinMode(joyXPin, INPUT);
  pinMode(joyYPin, INPUT);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT);
    buttons[i] = {LOW, LOW, LOW, 0};
  }

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

  // --- Joystick center calibration ---
  joyXFiltered = analogRead(joyXPin);
  joyYFiltered = analogRead(joyYPin);
  joyXCenter = joyXFiltered;
  joyYCenter = joyYFiltered;

  joyXRange = min(joyXCenter, (float)(4095.0) - joyXCenter);
  joyYRange = min(joyYCenter, (float)(4095.0) - joyYCenter);


  if (joyXRange < 1.0) joyXRange = 2048.0;
  if (joyYRange < 1.0) joyYRange = 2048.0;

  startTime = millis();
}

void loop() {
  unsigned long currentTime = millis();

  // --- Button Debouncing ---
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

    const float adjustedTolerance = 25;
    float safeLower = fmod(s.lower - adjustedTolerance + 360.0, 360.0);
    float safeRange = fmod(s.upper - safeLower + 360.0, 360.0);
    float safeDelta = fmod(s.w - safeLower + 360.0, 360.0);

    s.mapped = (safeDelta > safeRange) ? 1.0 : safeDelta / safeRange;
  }

  if (useOverride) generateOverrideValues();

  // --- Joystick smoothing ---
  int rawJoyX = analogRead(joyXPin);
  int rawJoyY = analogRead(joyYPin);

  float joyXNext = joystickAlpha * rawJoyX + (1.0 - joystickAlpha) * joyXFiltered;
  float joyYNext = joystickAlpha * rawJoyY + (1.0 - joystickAlpha) * joyYFiltered;

  if (abs(joyXNext - joyXFiltered) > joystickThreshold) joyXFiltered = joyXNext;
  if (abs(joyYNext - joyYFiltered) > joystickThreshold) joyYFiltered = joyYNext;

  // --- Normalize using calibrated center ---
  float joyXNormalized = (joyXFiltered - joyXCenter) / joyXRange;
  float joyYNormalized = (joyYFiltered - joyYCenter) / joyYRange;
  joyXNormalized = constrain(joyXNormalized, -1.0, 1.0);
  joyYNormalized = constrain(joyYNormalized, -1.0, 1.0);

  // --- Output ---
  char msg[128];
  int index = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    float val = useOverride ? overrideValues[i] : sensors[i].mapped;
    if (isnan(val)) val = 0.0;
    index += snprintf(msg + index, sizeof(msg) - index, "%.3f,", val);
  }
  index += snprintf(msg + index, sizeof(msg) - index, "%.2f,%.2f,", joyXNormalized, joyYNormalized);
  index += snprintf(msg + index, sizeof(msg) - index, "%d\n", buttons[0].debouncedState ? 0 : 1);

  BLEUartBufferedSend(msg);
  Serial.print(msg);

  delay(20);
}
