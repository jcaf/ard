#include <Ethernet.h>
#include <EthernetUdp.h>         
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);


byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 17);

unsigned int localPort = 8888;      

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

void setup() 
{
  lcd.begin(16, 2);
  
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  //Serial.begin(9600);
}

void loop() 
{
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    //Serial.println(packetSize);
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    lcd.setCursor(0,0);
    lcd.print(packetBuffer);
    lcd.print(" L/min");
  }
  delay(10);
}
