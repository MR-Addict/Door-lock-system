#include <EEPROM.h>

const int length = 11;
String UID[] = {
  "0A 57 80 1C", "FA 5D 70 7F", "D2 AA 1C 3E", "15 4F B9 E5", "0A 8C 36 B6", "00 69 0F 93",
  "75 89 B6 E5", "FA 50 35 B6", "EA B1 1C 3E", "60 DB 0D 93", "5A 32 34 B6", "72 B1 85 44",
  "A3 A4 1B 3E", "0C 3C 1A 3F", "72 B1 85 44", "50 DF D3 03", "A5 72 1B 3E", "A1 C2 53 74"
};

void erase(void)
{
  for (int i = 0 ; i < EEPROM.length() ; i++)
    EEPROM.write(i, 0);
}

void writeMsg(byte* first, size_t len, int j)
{
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(i + j * length, first[i]);
  }
}

void readMsg(size_t len, int j)
{
  byte res;

  Serial.print("Message: ");
  for (int i = 0; i < len; i++)
  {
    res = EEPROM.read(i + j * length);
    Serial.print((char)res);
  }
  Serial.println("");
}

void setup()
{
  Serial.begin(9600);
  
  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());
  Serial.print("Attempting to erase EEPROM... ");
  erase();
  Serial.print("Done!\n");
  Serial.print("Attempting to write to EEPROM...\n");

  for (int i = 0; i < sizeof(UID) / sizeof(String); i++) {
    String CONTECTED_UID = UID[i];
    char* string = CONTECTED_UID.c_str();
    writeMsg(string, length, i);
  }
  Serial.print("Done!\n");
  
  int pos;
  for (pos = 0; pos < EEPROM.length(); pos++)
    if (!EEPROM.read(pos))
      break;
  Serial.print("Attempting to read from EEPROM...\n");
  for (int i = 0; i < pos/11; i++) {
    readMsg(length, i);
  }
  Serial.print("Done!\n");
}

void loop(){
}
