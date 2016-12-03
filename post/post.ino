#include <Ethernet.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };
byte server[] = { 192,168,1,35 }; 

EthernetClient client;

void setup() 
{
  Serial.begin(9600);
  
  if(!Ethernet.begin(mac)) 
  {
    Serial.println("Could not configure Ethernet with DHCP");
  }
  else
  {
    Serial.println(Ethernet.localIP());
  }
envio_post();
}

void loop() 
{
  
   if (1)//(client.connected())
    {
        if (client.available())
        {
    /*
           if (1)//(client.find((char *)"<led_1>"))
            {
                int led_1 = client.parseInt();
                Serial.println(led_1);
            }
      */      
                char led_1 = client.read();
                Serial.print(led_1);
          // client.stop();
        }
          
      }
  //delay(2000);
}

void envio_post()
{
  if (client.connect(server, 80)) 
  {
    client.println("POST /ard.php HTTP/1.1");
    client.println("Host: 192.168.1.35");//obligado for HTTP 1.1.
    client.println("Content-Type: application/x-www-form-urlencoded; charset=utf-8");//client.println("Content-Type:text/html; charset=utf-8");
    client.println("Connection: close");
    client.println("User-Agent: FIRWAR/1.00");
    client.println("Content-Length: 10");
    client.println();
    client.print("valor=1981");
 //   Serial.println("conectado");
  }
  else
  {
    Serial.println("no connect");
  }
    
  
}


