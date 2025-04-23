#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

//#define DEBUG_MODE

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_TX   "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RX   "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
const int ledPin = 45;

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

#ifdef DEBUG_MODE
    Serial.print("In raw callback: ");
    for (size_t i = 0; i < len; ++i) {
      Serial.write(data[i]);
    }
    Serial.println();
#endif

    for (size_t i = 0; i < len && rxIndex < RX_BUFFER_SIZE - 1; ++i) {
      char c = (char)data[i];
      rxBuffer[rxIndex++] = c;
      if (c == '\n') {
        rxBuffer[rxIndex] = '\0';
        rxLineReady = true;
        rxIndex = 0;
        break;
      }
    }
  }
};

// Buffer outgoing data until '\n', then notify
void BLEUartBufferedSend(const char* msg) {
  if (!deviceConnected || !pTxCharacteristic) return;

  for (int i = 0; msg[i] != '\0'; ++i) {
    char c = msg[i];
    if (txIndex < TX_BUFFER_SIZE - 1) {
      txBuffer[txIndex++] = c;
    }

    if (c == '\n') {
      txBuffer[txIndex] = '\0';

#ifdef DEBUG_MODE
      Serial.println("Transmitting now...======");
      Serial.println(txBuffer);
      Serial.println("======");
#endif

      pTxCharacteristic->setValue((uint8_t*)txBuffer, txIndex);
      pTxCharacteristic->notify();
      txIndex = 0;
    }
  }
}

int BLEUartReceive(char* buf, int maxLen) {
  if (rxLineReady) {
    rxLineReady = false;
    int len = strlcpy(buf, rxBuffer, maxLen);
    return len;
  }
  return 0;
}

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32-BLE-UART");
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
}

bool led_state = false;
void loop() {
  char buf[RX_BUFFER_SIZE];
  int len = BLEUartReceive(buf, sizeof(buf));
  if (len > 0) {
    Serial.print("BLE RX: ");
    Serial.println(buf);

    // Transform: alternate uppercase
    for (int i = 0, cap = 1; i < len && buf[i] != '\0'; i++) {
      if (isalpha(buf[i])) {
        buf[i] = cap ? toupper(buf[i]) : tolower(buf[i]);
        cap = !cap;
      }
    }

    BLEUartBufferedSend(buf);  // Send transformed string
  }

  digitalWrite(ledPin, led_state ? HIGH : LOW);
  led_state = !led_state;

  delay(20);
}
