#include <Ethernet.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };
byte server[] = { 192,168,1,44}; 

EthernetClient client;

void setup() 
{
  Serial.begin(230400);
  
  if(!Ethernet.begin(mac)) 
  {
    Serial.println(F("Could not configure Ethernet with DHCP"));
  }
  else
  {
    Serial.println(Ethernet.localIP());
  }
  envio_post();
}

unsigned long last_millis;
void loop() 
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
  //delay(120000);
  
  //delay(5000);
}

void envio_post()
{
  if (client.connect(server, 80)) 
  {
    client.println(F("POST /jsondecode1.php HTTP/1.1"));
    client.println(F("Host: 192.168.1.44"));//obligado for HTTP 1.1.
    client.println(F("Content-Type: application/json"));//client.println("Content-Type: application/x-www-form-urlencoded; charset=utf-8");
    client.println(F("Connection: close"));
    client.println(F("User-Agent: FIRWAR/1.00"));
    //
    client.print(F("Content-Length: "));
    client.println(28);
    client.println();
    //
    client.println(F("{\"m1\":[[\"1\",\"2\"],[\"A\",\"R\"]]}"));//28

  //client.stop();
  }
  else
  {
    Serial.println(F("no connect"));
  }
    
  
}



