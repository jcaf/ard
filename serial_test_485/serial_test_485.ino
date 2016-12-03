//LECTURA DE SENSOR CLS-500
//JS67107
#define RS485_DIR 6
#define RS485_TX 1
#define RS485_RX 0

#define DEBUG_LED 39
void setup() 
{
  digitalWrite(DEBUG_LED,0);
  pinMode(DEBUG_LED, OUTPUT);
  
  digitalWrite(RS485_DIR, RS485_RX);
  pinMode(RS485_DIR, OUTPUT);
  
  Serial.begin(9600);
  Serial2.begin(9600);
  //digitalWrite(RS485_DIR, RS485_TX);
}

void loop() 
{
  //test1();
  test2();
}
void test1(void)
{
  static int sm=0;
  if (sm == 0)
  {
    digitalWrite(RS485_DIR, RS485_TX);
    sm++;
  }
  if (sm == 1) 
  {
    if (Serial.available()) 
    {
      int inByte = Serial.read();
      Serial2.write(inByte);
      
      if (inByte == '#')//ultime byte
      {
        //delayMicroseconds(50); //-> metodo 2
        delay(2);//-> metodo 1
        
        digitalWrite(RS485_DIR, RS485_RX);
        digitalWrite(DEBUG_LED, !digitalRead(DEBUG_LED));
        sm++;
      }
    }
  }
  
  if (sm == 2)
  {
    if (Serial2.available()) 
    {
      //digitalWrite(DEBUG_LED, !digitalRead(DEBUG_LED));
      int inByte = Serial2.read();
      Serial.write(inByte);
    }  
  }
}
////////////////////////////////////////////////////////////////
void test2(void)
{       
  static int sm=0;
  if (sm == 0)
  {
    digitalWrite(RS485_DIR, RS485_RX);
    sm++;
    }

  if (sm ==1)  
  {
    if (Serial2.available()) 
    {
      //digitalWrite(DEBUG_LED, !digitalRead(DEBUG_LED));
      int inByte = Serial2.read();
      Serial.write(inByte);
    }  
  }
 
  
}
