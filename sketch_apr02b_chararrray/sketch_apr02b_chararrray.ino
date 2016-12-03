#define LCD_COL 20
char buff[LCD_COL+1];
String s="01234567890123456789012345";
void setup() {
      
    if (s.length()>LCD_COL)
    {
        s.toCharArray(buff,18);//0..16
        buff[17]='.';
        buff[18]='.';
        buff[19]='.';
        buff[20]='\0';
    }
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
