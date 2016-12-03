#include <LiquidCrystal.h>

LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

#define LCD_PIN_BACKLIGHT 28

void setup() {
  // put your setup code here, to run once:
lcd.begin(20, 4);
lcd.setCursor(0,0);
digitalWrite(LCD_PIN_BACKLIGHT,HIGH);
pinMode(LCD_PIN_BACKLIGHT,OUTPUT);
for(int i=0; i<256; i++)
{
  lcd.write(i);
  delay(500);
}
}

void loop() {
  // put your main code here, to run repeatedly:

}
