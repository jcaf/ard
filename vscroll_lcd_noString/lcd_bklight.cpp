#include <Arduino.h>
#include "lcd_bklight.h"

#include "ikey.h"
#include <LiquidCrystal.h>
#include "system.h"

volatile uint16_t lcd_bklight_timmer=0x0000;
volatile uint8_t lcd_bklight_status=0;


void lcd_bklight_ON(void)
{
    digitalWrite(LCD_PIN_BACKLIGHT,HIGH);
    lcd_bklight_status = LCD_BKLIGHT_ON;
}
void lcd_bklight_OFF(void)
{
    digitalWrite(LCD_PIN_BACKLIGHT, LOW);
    lcd_bklight_status = LCD_BKLIGHT_OFF;
}

void lcd_bklight_timming_reset(void)
{
    lcd_bklight_timmer= 0x0000;
}

uint8_t lcd_bklight_get_status(void)
{
    return lcd_bklight_status;
}

void lcd_bklight_timming(void)//Asych -> ISR
{
    if (lcd_bklight_get_status() == LCD_BKLIGHT_ON)
    {
        if (++lcd_bklight_timmer > LCD_BKLIGHT_TIMMING_LIMIT)
        {
            lcd_bklight_timmer = 0x0000;

            lcd_bklight_OFF();
        }
    }
}

uint8_t lcd_bklight_is_ON(void)//Asych -> ISR
{
    static uint8_t sm0=0;
    uint8_t status=0;

    if (sm0 == 0)
    {
        if (lcd_bklight_get_status() == LCD_BKLIGHT_OFF)
        {
            if (!AllKeysReleased())
            {
                lcd_bklight_ON();
                sm0++;
            }

        }
        else
        {   status = 1;}
    }
    if (sm0 == 1)
    {
        if (AllKeysReleased())
        {
            status=1;
            sm0 = 0;
        }
    }
    return status;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
//Synch to main thread
///////////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_bklight_job(void)//Synch -> main
{
  if (lcd_bklight_get_status() == LCD_BKLIGHT_OFF)
    lcd.noDisplay();
  else
    lcd.display();
}





