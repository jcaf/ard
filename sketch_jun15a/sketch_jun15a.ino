void setup() {
  // put your setup code here, to run once:
Serial.begin(230400);
}


void loop() {
  // put your main code here, to run repeatedly:
 
  randomSeed(millis());   
  float randNumber_integer = random(11,14); 
 
 float randNumber_decimal = ((float)rand()/ (float)(RAND_MAX)) * 1.0;
      
      Serial.println(randNumber_integer+randNumber_decimal,3);

  delay(1200);
}
