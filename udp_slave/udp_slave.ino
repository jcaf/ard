#include <SPI.h>        
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = {  
  0xDF, 0xA1, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
#define localPort 8888

IPAddress ip_master(192, 168, 1, 17);
#define localPort_master 8888


EthernetUDP Udp;

void setup() 
{
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
}

float caudal = 1234.45;
char buff[10];
void loop() 
{
  Udp.beginPacket(ip_master, localPort_master);
  dtostrf(caudal,5,2,buff);
  Udp.write(buff);
  Udp.endPacket();

}
