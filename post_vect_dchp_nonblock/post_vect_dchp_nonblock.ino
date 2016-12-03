#if defined(ARDUINO) && ARDUINO > 18
#include <SPI.h>
#endif
#include <Ethernet.h>
#include <EthernetDHCP.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

const char* ip_to_str(const uint8_t*);

//#include <Ethernet.h>

//byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };
byte server[] = { 192,168,1,42}; 

EthernetClient client;

void setup() 
{
  Serial.begin(230400);
  
   EthernetDHCP.begin(mac, 1);
}

unsigned long last_millis;

int eth_init=0;
void loop() 
{
   static DhcpState prevState = DhcpStateNone;
  static unsigned long prevTime = 0;
/*
  if (Serial.available())
  {
    char rx = Serial.read();
    if (rx == 'r')
    {
      EthernetDHCP.begin(mac, 1);
      Serial.println("buscando nuevamente IP");
      
      client.stop();
      eth_init = 0;
     
     }
      
    }
  */
  DhcpState state = EthernetDHCP.poll();
  if (prevState != state) 
  {
    Serial.println();

    switch (state) {
      case DhcpStateDiscovering:
        Serial.print("Discovering servers.");
        break;
      case DhcpStateRequesting:
        Serial.print("Requesting lease.");
        break;
      case DhcpStateRenewing:
        Serial.print("Renewing lease.");
        break;
      case DhcpStateLeased: {
        Serial.println("Obtained lease!");

        // Since we're here, it means that we now have a DHCP lease, so we
        // print out some information.
        const byte* ipAddr = EthernetDHCP.ipAddress();
        const byte* gatewayAddr = EthernetDHCP.gatewayIpAddress();
        const byte* dnsAddr = EthernetDHCP.dnsIpAddress();

        Serial.print("My IP address is ");
        Serial.println(ip_to_str(ipAddr));

        Serial.print("Gateway IP address is ");
        Serial.println(ip_to_str(gatewayAddr));

        Serial.print("DNS IP address is ");
        Serial.println(ip_to_str(dnsAddr));

        Serial.println();
        eth_init = 1;
        break;
      }
    }
  } else if (state != DhcpStateLeased && millis() - prevTime > 300) {
     prevTime = millis();
     Serial.print('.'); 
  }

  prevState = state;
  
  /////////////////////////////////
    if (eth_init)
    {
    envio_post();
    while (client.connected())
    {
        if (client.available())
        {
              char rx= client.read();
              Serial.print(rx);
              if (rx == '?')
              {
                client.stop();
              }
        }
      }
  delay(1000);
    }
}

void envio_post()
{
  if (client.connect(server, 80)) 
  {
    client.println(F("POST /jsondecode.php HTTP/1.1"));
    client.println(F("Host: 192.168.1.42"));//obligado for HTTP 1.1.
    client.println(F("Content-Type: application/json"));//client.println("Content-Type: application/x-www-form-urlencoded; charset=utf-8");
    client.println(F("Connection: close"));
    client.println(F("User-Agent: FIRWAR/1.00"));
    //
    client.print(F("Content-Length: "));
    client.println(28);
    client.println();
    //
    client.println(F("{\"m1\":[[\"1\",\"2\"],[\"A\",\"R\"]]}"));//28

  }
  else
  {
    Serial.println(F("no connect"));
    
    client.stop();
    EthernetDHCP.begin(mac, 1);
     Serial.println("buscando nuevamente IP");
      eth_init = 0;
  }
    
  
}


// Just a utility function to nicely format an IP address.
const char* ip_to_str(const uint8_t* ipAddr)
{
  static char buf[16];
  sprintf(buf, "%d.%d.%d.%d\0", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
  return buf;
}
