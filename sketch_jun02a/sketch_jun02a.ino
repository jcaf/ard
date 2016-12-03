
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };
byte server[] = { 192,168,1,44 }; 

EthernetClient client;

void setup() {

}

void loop() {

}

void envio()
{
  if (client.connect(server, 80)) 
  {
    "GET /sensorarduino.php?id=joniuz&nombre=temperatura&valor=18.55 HTTP/1.1" 200 - "-" "Arduino-Ethernet"
  }
}
