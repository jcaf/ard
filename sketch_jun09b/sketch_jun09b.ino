
void setup() 
{
  char buf[12];
  // put your setup code here, to run once:
  float f=+12;
  dtostrf(f,4,3,buf);
  Serial.begin(230400);
  Serial.print(buf);
}

void loop() {
  // put your main code here, to run repeatedly:

}
