struct _x
{
    int a;
}x[2]=
{
  x[0].a=3,
  x[1].a=5
};

    

void setup() {
  // put your setup code here, to run once:
Serial.begin(230400);
Serial.print(x[0].a + String( x[1].a) );
}

void loop() {
  // put your main code here, to run repeatedly:

}
