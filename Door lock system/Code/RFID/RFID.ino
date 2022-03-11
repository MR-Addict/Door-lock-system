// Header files for NFC
#include <SPI.h>
#include <MFRC522.h>
#include <AccelStepper.h>

// Header files for WIFI
#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

const uint8_t PinA = 14;
const uint8_t PinB = 27;
const uint8_t PinC = 26;
const uint8_t PinD = 25;
const uint8_t Buzzer = 12;
const uint8_t SS_PIN = 5;
const uint8_t RST_PIN = 22;
const uint8_t INT_PIN = 4;

bool isDetectedCard = false;
const int16_t stepsPerRevolution = -500;

const uint8_t UID[][4] = {
    {0xF1, 0x41, 0x01, 0x5F}, {0x01, 0x5E, 0x95, 0x2B}, {0x1A, 0xFD, 0xA5, 0x21}, {0xE1, 0xD8, 0xFC, 0x5E}, {0xE1, 0xE0, 0x00, 0x5F},
    {0xD1, 0x7E, 0xFF, 0x5E}, {0x21, 0xDE, 0x01, 0x5F}, {0xD1, 0x3A, 0xF6, 0x5E}, {0x31, 0xAB, 0xFD, 0x5E}, {0x21, 0x35, 0xF7, 0x5E},
    {0x0A, 0x57, 0x80, 0x1C}, {0xFA, 0x5D, 0x70, 0x7F}, {0xD2, 0xAA, 0x1C, 0x3E}, {0x15, 0x4F, 0xB9, 0xE5}, {0x75, 0x89, 0xB6, 0xE5},
    {0x0A, 0x8C, 0x36, 0xB6}, {0x00, 0x69, 0x0F, 0x93}, {0x75, 0x89, 0xB6, 0xE5}, {0xFA, 0x50, 0x35, 0xB6}, {0xEA, 0xB1, 0x1C, 0x3E},
    {0x60, 0xDB, 0x0D, 0x93}, {0x5A, 0x32, 0x34, 0xB6}, {0x72, 0xB1, 0x85, 0x44}, {0xA3, 0xA4, 0x1B, 0x3E}, {0x0C, 0x3C, 0x1A, 0x3F},
    {0x72, 0xB1, 0x85, 0x44}, {0x50, 0xDF, 0xD3, 0x03}, {0xA5, 0x72, 0x1B, 0x3E}, {0xA1, 0xC2, 0x53, 0x74}, {0x6C, 0xAE, 0x73, 0x17},
    {0x0A, 0x46, 0x95, 0x1C}, {0x47, 0x25, 0x38, 0xE1}, {0xDC, 0x53, 0x14, 0x3E}, {0x80, 0x54, 0x0C, 0x93}, {0x2A, 0x76, 0x64, 0x21},
    {0xDC, 0xFB, 0x0E, 0x3F}, {0x5A, 0x20, 0x4C, 0x2D},
};

MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
AccelStepper stepper(8, PinA, PinC, PinB, PinD);

AsyncWebServer server(80);
WebSocketsServer websocket(81);

void ringBuzzer(uint8_t);
void UID_ISR();
bool checkUID();
void openDoor();

// Header files for custom function
#include "arduino_secrets.h"
#include "WIFI_WS.h"

void setup() {
    Serial.begin(115200);
    SPI.begin();
    pinMode(Buzzer, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(INT_PIN), UID_ISR, FALLING);
    rfid.PCD_Init();

    stepper.setMaxSpeed(500.0);
    stepper.setAcceleration(1000.0);
    stepper.runToNewPosition(0);

    WIFI_Init();
    Server_Init();

    ringBuzzer(100);
}

void loop() {
    websocket.loop();
    readCard();
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

void openDoor() {
    ringBuzzer(100);
    stepper.runToNewPosition(stepsPerRevolution);
    delay(1000);
    stepper.runToNewPosition(0);
}

void readCard() {
    if (!isDetectedCard)return;
    delay(1000);
    if (!rfid.PICC_IsNewCardPresent())return;
    if (!rfid.PICC_ReadCardSerial())return;

    if (checkUID()) {
        openDoor();
    }
    else {
        ringBuzzer(200);
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    isDetectedCard = false;
}
