/*
 * *****************************************************************************
 * See tutorial: https://ODev.atlassian.net/wiki/display/DOC/A.+First+Steps+with+ODev
 * This example code is in the public domain.
 * *****************************************************************************
 * 
 */

//#include <UIPEthernet.h> // Uncomment for use: ENC28J60
//#include <YunServer.h> //  Uncomment for: Arduino Yun Bridge(port: 5555)
#include <Ethernet.h>
#include <SPI.h>
#include <OpenDevice.h>

void setup(){
  
  ODev.id(ODev.generateID(A0)); // is the MAC (it's saved on EEPROM)
  ODev.name("Module-Thing1");
  // ODev.ip(192,168,3,106);
 
  ODev.enableDebug();
  ODev.addDevice(13, Device::DIGITAL);
  ODev.addCommand("mycmd1", ledON);
  ODev.addCommand("mycmd2", ledOFF);
  ODev.begin();
}

void loop(){
  ODev.loop();
}

// SEND: 99;0;mycmd1;MyString;10
void ledON(){
  ODev.debug(ODev.readString());
  int count = ODev.readInt();
  for (int i = 0; i < count; i++) {
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
    delay(200);
  }
  digitalWrite(13, HIGH);
}

// SEND: 99;0;mycmd2
void ledOFF(){
  digitalWrite(13, LOW);
}
