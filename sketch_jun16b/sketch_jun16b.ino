//const char v[] = "hola";
const char v[] PROGMEM = "hola";
#define cc sizeof(v)/sizeof(v[0])

void setup() 
{
  char buf[cc];
  strcpy_P(buf, (char * ) &v );
  
Serial.begin(230400);
Serial.print( cc );
Serial.print( buf );

}

void loop() {
  // put your main code here, to run repeatedly:

}
