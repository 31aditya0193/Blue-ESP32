#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// ------------------ Hardware Pins ------------------
#define RED_PIN   27
#define GREEN_PIN 26
#define BLUE_PIN  25

// PWM settings
#define PWM_FREQ    5000
#define PWM_RESOLUTION 8  // 8-bit: 0-255
#define RED_CHANNEL   0
#define GREEN_CHANNEL 1
#define BLUE_CHANNEL  2

// Screen setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ------------------ BLE UUIDs ------------------
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define LED_CHAR_UUID       "beb5483e-36e1-4688-b7f5-ea07361b26a8"  // RGB: 3 bytes
#define TEXT_CHAR_UUID      "1c95d5e3-d8f7-413a-bf3d-7d3d930a6d48" // Text string

BLECharacteristic *pLedCharacteristic = nullptr;
BLECharacteristic *pTextCharacteristic = nullptr;
bool deviceConnected = false;

// ------------------ Callbacks ------------------
class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
    BLEDevice::startAdvertising();  // Restart advertising
  }
};

class LedWriteCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) {
    std::string value = pChar->getValue();
    if (value.length() == 3) {
      uint8_t r = value[0];
      uint8_t g = value[1];
      uint8_t b = value[2];

      // For common cathode: higher value = brighter
      ledcWrite(RED_CHANNEL,   r);
      ledcWrite(GREEN_CHANNEL, g);
      ledcWrite(BLUE_CHANNEL,  b);

      Serial.printf("LED set to RGB(%d, %d, %d)\n", r, g, b);
    }
  }
};

class TextWriteCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) {
    std::string value = pChar->getValue();
    if (value.length() > 0) {
      String text = String(value.c_str());
      Serial.println("Text received: " + text);

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);

      // Simple word wrap for longer text
      int line = 0;
      int maxCharsPerLine = 21;  // Adjust based on your font/screen
      for (size_t i = 0; i < text.length(); i += maxCharsPerLine) {
        String substring = text.substring(i, i + maxCharsPerLine);
        display.setCursor(0, line * 10);
        display.println(substring);
        line++;
        if (line * 10 >= SCREEN_HEIGHT) break;  // Prevent overflow
      }

      display.display();
    }
  }
};

// ------------------ Setup ------------------
void setup() {
  Serial.begin(115200);

  // --- Initialize RGB LED PWM ---
  ledcSetup(RED_CHANNEL,   PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(GREEN_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(BLUE_CHANNEL,  PWM_FREQ, PWM_RESOLUTION);

  ledcAttachPin(RED_PIN,   RED_CHANNEL);
  ledcAttachPin(GREEN_PIN, GREEN_CHANNEL);
  ledcAttachPin(BLUE_PIN,  BLUE_CHANNEL);

  // Start with LED off
  ledcWrite(RED_CHANNEL,   255);
  ledcWrite(GREEN_CHANNEL, 0);
  ledcWrite(BLUE_CHANNEL,  0);

  // --- Initialize OLED ---
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) delay(100);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("BLE Ready");
  display.display();

  // --- Initialize BLE ---
  BLEDevice::init("ESP32_RGB_Display");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // LED Characteristic
  pLedCharacteristic = pService->createCharacteristic(
    LED_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pLedCharacteristic->setCallbacks(new LedWriteCallback());

  // Text Characteristic
  pTextCharacteristic = pService->createCharacteristic(
    TEXT_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pTextCharacteristic->setCallbacks(new TextWriteCallback());

  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("BLE Advertising started...");
}

void loop() {
  // You can add other tasks here (e.g., sensor reading, animations)
  delay(100);
}