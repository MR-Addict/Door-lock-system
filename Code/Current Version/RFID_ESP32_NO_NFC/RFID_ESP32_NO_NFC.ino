// Header files for NFC
#include <AccelStepper.h>

// Header files for WIFI
#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

const uint8_t PinA = 14;
const uint8_t PinB = 27;
const uint8_t PinC = 26;
const uint8_t PinD = 25;
const uint8_t Buzzer = 12;

volatile boolean isOpenDoor = false;
const int16_t stepsPerRevolution = -500;

AccelStepper stepper(8, PinA, PinC, PinB, PinD);
AsyncWebServer server(80);

void ringBuzzer(uint8_t);
void openDoor();

// Header files for custom function
#include "arduino_secrets.h"
#include "WiFi_Server.h"

void setup() {
  Serial.begin(115200);
  pinMode(Buzzer, OUTPUT);

  stepper.setMaxSpeed(500.0);
  stepper.setAcceleration(1000.0);
  stepper.runToNewPosition(0);

  WIFI_Init();
  Server_Init();

  ringBuzzer(100);
}

void loop() {
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

void openDoor() {
  ringBuzzer(100);
  stepper.runToNewPosition(stepsPerRevolution);
  delay(1000);
  stepper.runToNewPosition(0);
  isOpenDoor = false;
}
