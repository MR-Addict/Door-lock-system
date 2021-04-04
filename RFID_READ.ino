#include <EEPROM.h>

const int length = 11;

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

void loop()
{ }
