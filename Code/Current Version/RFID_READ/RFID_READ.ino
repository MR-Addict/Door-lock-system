#include <SPI.h>
#include <MFRC522.h>

const uint8_t SS_PIN = 5;
const uint8_t RST_PIN = 22;

MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

void setup() {
    Serial.begin(115200);
    SPI.begin();
    rfid.PCD_Init();
    rfid.PCD_DumpVersionToSerial();
}

void loop() {
    if (!rfid.PICC_IsNewCardPresent())return;
    if (!rfid.PICC_ReadCardSerial())return;

    Serial.print('{');
    for (uint8_t i = 0; i < 4; i++) {
        Serial.print("0x");
        if (rfid.uid.uidByte[i] < 0x10)Serial.print('0');
        Serial.print(rfid.uid.uidByte[i], HEX);
        if (i != 3)Serial.print(", ");
    }
    Serial.print("}\n");

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}
