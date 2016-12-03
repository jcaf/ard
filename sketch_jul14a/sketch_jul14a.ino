int a=1;
int b=2;
void setup() 
{
  Serial.begin(230400);
  
  int c;
  c = (a>2)? 5:7;

  //Serial.println(c);


// put your setup code here, to run once:
//char buff[10]={'x','x','x','x','x','x','x','x','x','x',};
char buff[7]={'x','x','x','x','x','x','x',};

dtostrf(12.45, 0 ,2, buff);


for (int i=0; i<7; i++)
{
   if (buff[i]== 0) 
    Serial.println("CERO");
    else
    Serial.println(buff[i]);
}

   

}

void loop() {
  // put your main code here, to run repeatedly:

}
