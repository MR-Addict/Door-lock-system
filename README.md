# Door-lock-system
Door lock system using RFID, servo motor, buzzer and arduino.
This system features saving the card information forever even after powr off,writing and erasing cards easily
only using a "master UID card".

In order to make the system able to write and delete the UID card, <EEPROM.h> library must be included.
With it we can make sure that the data wouldnl't get lost.
