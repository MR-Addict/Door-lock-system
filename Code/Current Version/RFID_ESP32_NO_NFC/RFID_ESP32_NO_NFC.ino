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
const uint8_t RESET_INT_PIN = 0;

boolean isAPMode = false;
volatile boolean isOpenDoor = false;
const int16_t stepsPerRevolution = -500;

// WiFi configuration structure
struct WiFiData {
  String ssid;
  String pwd;
  String hostname;
  String login_user;
  String login_pwd;
};

WiFiData wifi;
AccelStepper stepper(8, PinA, PinC, PinB, PinD);
AsyncWebServer server(80);

void ringBuzzer(uint8_t);
void openDoor();

// Header files for custom function
#include "SPIFFS_Handler.h"
#include "WiFi_Server.h"

void setup() {
  Serial.begin(115200);
  pinMode(Buzzer, OUTPUT);
  pinMode(RESET_INT_PIN, INPUT_PULLUP);

  stepper.setMaxSpeed(500.0);
  stepper.setAcceleration(1000.0);
  stepper.runToNewPosition(0);

  initSPIFFS();
  if (updateSPIFFS()) {
    setSTA();
    isAPMode = false;
    ringBuzzer(100);
  } else {
    setAP();
    isAPMode = true;
    ringBuzzer(250);
  }
  Server_Init();
}

void loop() {
  if (!digitalRead(RESET_INT_PIN)) {
    delay(10);
    if (digitalRead(RESET_INT_PIN))return;
    unsigned long lastTime = millis();
    while (!digitalRead(RESET_INT_PIN) && millis() - lastTime < 2000);
    Serial.println("Set AP Mode!");
    setAP();
    isAPMode = true;
    ringBuzzer(250);
  }
  else if (isOpenDoor)openDoor();
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
