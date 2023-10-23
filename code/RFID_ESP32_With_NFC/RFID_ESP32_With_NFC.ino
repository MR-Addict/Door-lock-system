// Header files for NFC
#include <SPI.h>
#include <MFRC522.h>
#include <AccelStepper.h>

// Header files for WIFI
#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

const uint8_t PinA = 14;
const uint8_t PinB = 27;
const uint8_t PinC = 26;
const uint8_t PinD = 25;
const uint8_t Buzzer = 12;
const uint8_t SS_PIN = 5;
const uint8_t RST_PIN = 22;
const uint8_t INT_PIN = 4;

const String LOGIN_USER = "admin";
const String LOGIN_PASSWORD = "stas";
const String WIFI_SSID = "STAS-507";
const String WIFI_PASSWORD = "manke666";

volatile boolean isOpenDoor = false;
volatile boolean isDetectedCard = false;
const int16_t stepsPerRevolution = 4096;

const uint8_t UID[][4] = {
  // 蔡建文
  {0xEE, 0xC5, 0xE1, 0x1D},
  // 童浩然
  {0x70, 0x33, 0x88, 0x56}
};

MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
AccelStepper stepper(8, PinA, PinC, PinB, PinD);
AsyncWebServer server(80);

void ringBuzzer(uint8_t);
void UID_ISR();
bool checkUID();
void openDoor();

// Header files for custom function
#include "WIFI_WS.h"

void setup() {
  SPI.begin();
  Serial.begin(115200);
  pinMode(Buzzer, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), UID_ISR, FALLING);
  rfid.PCD_Init();

  stepper.setMaxSpeed(1500.0);
  stepper.setAcceleration(1000.0);
  stepper.runToNewPosition(0);

  WIFI_Init();
  Server_Init();

  ringBuzzer(100);
}

void loop() {
  readCard();
  if (isOpenDoor)openDoor();
}

void ringBuzzer(uint8_t duration) {
  for (uint8_t i = 0; i < 2; i++) {
    digitalWrite(Buzzer, HIGH);
    delay(duration);
    digitalWrite(Buzzer, LOW);
    delay(100);
  }
}

void UID_ISR() {
  isDetectedCard = true;
}

bool checkUID() {
  for (uint8_t i = 0; i < sizeof(UID) / (sizeof(uint8_t) * 4); i++) {
    for (uint8_t j = 0, k = 0; j < 4; j++) {
      if (rfid.uid.uidByte[j] != UID[i][j])k++;
      if (!k)return true;
    }
  }
  return false;
}

void readCard() {
  // uncomment it for interrupt detect
  // if (!isDetectedCard)return;
  // delay(300);
  if (!rfid.PICC_IsNewCardPresent())return;
  if (!rfid.PICC_ReadCardSerial())return;

  if (checkUID()) {
    isOpenDoor = true;
  }
  else {
    ringBuzzer(200);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  isDetectedCard = false;
}

void openDoor() {
  ringBuzzer(100);
  stepper.runToNewPosition(stepsPerRevolution);
  delay(1000);
  stepper.runToNewPosition(0);
  isOpenDoor = false;
}
