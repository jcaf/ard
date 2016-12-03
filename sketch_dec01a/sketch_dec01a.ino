void setup() {
  // put your setup code here, to run once:
pinMode(2,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
 for (int i=0; i<300; i++)
 {
  digitalWrite(2,1);
  delayMicroseconds(500);
   
  //delay
   digitalWrite(2,0);
   delayMicroseconds(500);
  }
  
  digitalWrite(2,1);
  delay(3);
  digitalWrite(2,0);

  delay(1000);
 

}
