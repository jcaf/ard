void setup() 
{
  // put your setup code here, to run once:
Serial.begin(9600);
String a="amor";
fx(&a);
Serial.println("el valor de a es");
 Serial.println(a);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void fx(String * s)
{
  Serial.println(*s);
  *s = "xxxx";
  Serial.println(*s);
}
