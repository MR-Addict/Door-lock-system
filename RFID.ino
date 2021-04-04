#include <SPI.h>
#include <Servo.h>
#include <MFRC522.h>
#include <EEPROM.h>

int position;
const int SS_PIN = 10;
const int RST_PIN = 9;
const int buzzerPin = 7;
const int length = 11;
const int numUID = 93;
const String masterUID = "0A 46 95 1C";
String UID[numUID] = {
};

Servo myservo;
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Put your card to the reader...");
  Serial.println();
  myservo.attach(8);
  myservo.write(90);
  updateUID();
  pinMode(buzzerPin, OUTPUT);
  noTone(buzzerPin);
  for (position; position < EEPROM.length(); position++)
    if (!EEPROM.read(position))
      break;
}

void loop()
{
  //启动RFID
  if ( ! mfrc522.PICC_IsNewCardPresent())return;
  if ( ! mfrc522.PICC_ReadCardSerial())return;

  //识别UID
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  //识别是否为内存用户
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  String CONTECTED_UID = content.substring(1);
  int flag = 0;
  if (CONTECTED_UID == masterUID)flag = 2;
  else {
    for (int i = 0; i < position / 11; i++) {
      if (CONTECTED_UID == UID[i]) {
        flag = 1;
        break;
      }
    }
  }
  if (flag == 0) {
    Serial.println("Denied!");
    tone(buzzerPin, 300);
    delay(1000);
    noTone(buzzerPin);
  }

  else if (flag == 1) {
    Serial.println("Accepted!");
    for (int pos = 90; pos >= 0; pos -= 1) {
      myservo.write(pos);
      delay(5);
    }
    for (int pos = 0; pos <= 90; pos += 1) {
      myservo.write(pos);
      delay(5);
    }
  }

  else if (flag == 2) {
    tone(buzzerPin, 300);
    delay(300);
    noTone(buzzerPin);
    masterUIDPrivilege();
  }
}

void masterUIDPrivilege() {
  Serial.println("Waiting for new UID card approching...");
  delay(2000);
  while (! mfrc522.PICC_IsNewCardPresent());
  while (! mfrc522.PICC_ReadCardSerial());
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  String CONTECTED_UID = content.substring(1);
  char* string = CONTECTED_UID.c_str();
  int lable = 0;
  if (CONTECTED_UID == masterUID)lable = 1;
  else {
    for (int i = 0; i < position / 11; i++) {
      if (CONTECTED_UID == UID[i]) {
        lable = 2;
        break;
      }
    }
  }
  if (lable == 0) {
    addUID(string);
    Serial.println("UID successfully added!");
    tone(buzzerPin, 300);
    delay(300);
    noTone(buzzerPin);
    delay(1000);
    updateUID();
  }
  else if (lable == 1) {
    Serial.println("Failed!");
    tone(buzzerPin, 300);
    delay(1000);
    noTone(buzzerPin);
  }
  else if (lable == 2) {
    removeUID(string);
    Serial.println("UID successfully removed!");
    tone(buzzerPin, 300);
    delay(1000);
    noTone(buzzerPin);
    updateUID();
  }
}

void updateUID() {
  int pos;
  for (pos = 0; pos < EEPROM.length(); pos++)
    if (!EEPROM.read(pos))
      break;
  for (int i = 0; i <= pos / 11; i++) {
    String string;
    for (int j = 0; j < length; j++) {
      string += (char)EEPROM.read(j + i * length);
    }
    UID[i] = string;
  }
  position = pos;
}

void addUID(byte * first)
{
  int pos;
  for (pos = 0; pos < EEPROM.length(); pos++)
    if (!EEPROM.read(pos))
      break;
  for (int i = 0; i < length; i++)
  {
    EEPROM.write(i + pos, first[i]);
  }
}

void removeUID(String string) {
  int pos = -1;
  for (int i = 0; i < position / 11; i++) {
    if (UID[i] == string) {
      pos = i;
      break;
    }
  }
  if (pos >= 0 ) {
    for (int i = pos; i <= position / 11; i++) {
      for (int j = 0; j < length; j++) {
        EEPROM.write(j + i * length, EEPROM.read(j + (i + 1)*length));
      }
    }
  }
}
