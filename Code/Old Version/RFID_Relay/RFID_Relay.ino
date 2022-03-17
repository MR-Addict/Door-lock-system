#include <SPI.h>
#include <MFRC522.h>

const int SS_PIN = 10;
const int RST_PIN = 9;
const int Buzzer = 8;
const int Relay = 7;
const int UIDNUM = 50;

const String UID[UIDNUM] = {
    "F1 41 01 5F", "01 5E 95 2B", "1A FD A5 21", "E1 D8 FC 5E", "E1 E0 00 5F",
    "D1 7E FF 5E", "21 DE 01 5F", "D1 3A F6 5E", "31 AB FD 5E", "21 35 F7 5E",
    "0A 57 80 1C", "FA 5D 70 7F", "D2 AA 1C 3E", "15 4F B9 E5", "75 89 B6 E5",
    "0A 8C 36 B6", "00 69 0F 93", "75 89 B6 E5", "FA 50 35 B6", "EA B1 1C 3E",
    "60 DB 0D 93", "5A 32 34 B6", "72 B1 85 44", "A3 A4 1B 3E", "0C 3C 1A 3F",
    "72 B1 85 44", "50 DF D3 03", "A5 72 1B 3E", "A1 C2 53 74", "6C AE 73 17",
    "0A 46 95 1C", "47 25 38 E1", "DC 53 14 3E", "80 54 0C 93", "2A 76 64 21",
    "10 FD FC 2E", "19 F0 95 B4", "D7 FC 80 67", "60 B9 2A BE", "75 36 B5 E5",
    "87 90 91 5E", "69 E9 1F C5", "C9 DE 3B B4", "08 71 21 DC", "9C B3 BC 3E",
    "17 A4 91 5E", "F7 88 94 5E", "89 28 3A B4", "BC C3 87 43", "DC FB 0E 3F",
};

MFRC522 mfrc522(SS_PIN, RST_PIN);

void BuzzerRing(boolean Dur = false) {
    for (uint8_t i = 0; i < 2; i++) {
        tone(Buzzer, 300);
        if (Dur)delay(200);
        else delay(100);
        noTone(Buzzer);
        delay(100);
    }
}

boolean GetKey(String CONTECTED_UID) {
    for (uint8_t i = 0; i < UIDNUM; i++)
        if (CONTECTED_UID == UID[i])return true;
    return false;
}

void setup()
{
    Serial.begin(115200);
    SPI.begin();
    mfrc522.PCD_Init();
    pinMode(Relay, OUTPUT);
    pinMode(Buzzer, OUTPUT);
    BuzzerRing();
    Serial.print("System Started!\nBuilt in UID Num:");
    Serial.println(UIDNUM);
    delay(3000);
}

void loop()
{
    if ( ! mfrc522.PICC_IsNewCardPresent())return;
    if ( ! mfrc522.PICC_ReadCardSerial())return;

    String content = "";
    Serial.print("UID(");
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    String CONTECTED_UID = content.substring(1);

    if (GetKey(CONTECTED_UID)) {
        Serial.println(" ):Access Accpeted!");
        BuzzerRing();
        digitalWrite(Relay, HIGH);
        delay(1000);
        digitalWrite(Relay, LOW);
    }
    else {
        Serial.println(" ):Access Denied!");
        BuzzerRing(true);
    }
}
