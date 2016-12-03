#include <LiquidCrystal.h>

 #define LCD_PIN_BACKLIGHT 28
 
class mylcd: public LiquidCrystal
{
  using LiquidCrystal::LiquidCrystal;
  public:
  void lcd_print_at_posc(uint8_t fil, uint8_t col, String s, String s1="", long time=0 );
  };

mylcd lcd(22, 23, 24, 25, 26, 27);

void mylcd::lcd_print_at_posc(uint8_t fil, uint8_t col, String s, String s1, long time)
{
   setCursor(col,fil);
   print(s);
   if (time>0)
   {
    delay(time);
    print("change");
    }
  }

void setup() {
      lcd.begin(20, 4);
   digitalWrite(LCD_PIN_BACKLIGHT,HIGH);
   pinMode(LCD_PIN_BACKLIGHT,OUTPUT);
    
  lcd.lcd_print_at_posc(1,0,"HELLO",,1000);
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
