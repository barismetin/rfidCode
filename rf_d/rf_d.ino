#include <MFRC522.h>
//Get library from https://github.com/ljos/MFRC522
//Sketch: gudjonholm@gmail.com
#include <SPI.h>

/*
Pins  SPI    UNO  Mega2560  Leonardo
1 (NSS) SAD (SS)   10     53        10
2       SCK        13     52        SCK1
3       MOSI       11     51        MOSI1
4       MISO       12     50        MISO1
5       IRQ        *      *         *
6       GND       GND     GND       GND
7       RST        5      ?         Reset
8      +3.3V (VCC) 3V3     3V3       3.3V
* Not needed
1 on ICPS header
*/
#define SAD 10
#define RST 5
MFRC522 nfc(SAD, RST);


#define ledPinOpen    2
#define ledPinClosed  3


void setup() {
  pinMode(ledPinOpen  , OUTPUT);   
  pinMode(ledPinClosed, OUTPUT);   
  SPI.begin();
  Serial.begin(115200);
  Serial.println("Looking for MFRC522.");
  nfc.begin();
  byte version = nfc.getFirmwareVersion();
  if (! version) {
    Serial.print("Didn't find MFRC522 board.");
    while(1); //halt
  }

  Serial.print("Found chip MFRC522 ");
  Serial.print("Firmware ver. 0x");
  Serial.print(version, HEX);
  Serial.println(".");
}

#define AUTHORIZED_COUNT 1 /*If you want more Authorized of cards set the count here, and then add the serials below*/
byte Authorized[AUTHORIZED_COUNT][6] = {

                            {0x45, 0x14, 0x39, 0x2E, 0xFF, 0xFF, }
                           /*,{0x10, 0x14, 0x39, 0x2E, 0xFF, 0xFF, }*/  /*f. example how to add more authorized cards*/
                            
                          };
                          
void printSerial(byte *serial);
boolean isSame(byte *key, byte *serial);
boolean isAuthorized(byte *serial);



void loop() {
  byte status;
  byte data[MAX_LEN];
  byte serial[5];
  boolean Opening = false;
  digitalWrite(ledPinOpen, Opening);
  digitalWrite(ledPinClosed, !Opening);
  status = nfc.requestTag(MF1_REQIDL, data);

  if (status == MI_OK) {
    status = nfc.antiCollision(data);
    memcpy(serial, data, 5);
    
    if(isAuthorized(serial))
    { 
      Serial.println("Authenticated");
      Opening = true;
    }
    else
    { 
      printSerial(serial);
      Serial.println("is NOT authenticated");
      Opening = false;
    }
    
    nfc.haltTag();
    digitalWrite(ledPinOpen, Opening);
    digitalWrite(ledPinClosed, !Opening);
    delay(2000);
  }//if (status == MI_OK)

  delay(500);

}//void loop()

boolean isSame(byte *key, byte *serial)
{
    for (int i = 0; i < 4; i++) {
      if (key[i] != serial[i])
      { 
        return false; 
      }
    }
    
    return true;

}

boolean isAuthorized(byte *serial)
{
    for(int i = 0; i<AUTHORIZED_COUNT; i++)
    {
      if(isSame(serial, Authorized[i]))
        return true;
    }
   return false;
}

void printSerial(byte *serial)
{
        Serial.print("Serial:");
    for (int i = 0; i < 4; i++) {
      Serial.print(serial[i], HEX);
      Serial.print(" ");
    }
}
