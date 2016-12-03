#include <LiquidCrystal.h>
#include "system.h"
#include "LTC68042.h"
#include "ltc6804.h"
#include "ikey.h"
#include "kb.h"
#include "lcd_bklight.h"
#include "lcd_fxaux.h"
#include "vscroll.h"

#include "menu_mainMenu.h"
#include "menu_bkground.h"

#include "rele.h"
#include "digital_input.h"
#include "analog_input.h"
#include "leds.h"

#include "errorsys.h"
#include "boot.h"

#include "ethernet.h"
#include "DS3231.h"
#include "I2C.h"

#include "time.h"
#include "date.h"
#include "rtcc.h"
#include "fuelsensor.h"
#include "fuelsensor_jt606x.h"

//LiquidCrystal lcd(22, 23, 24, 25, 26, 27);
mylcd lcd(22, 23, 24, 25, 26, 27);

struct _scheduler scheduler;

void setup()
{
    //jt606_run();

    lcd.begin(LCD_COL, LCD_ROW);
    lcd.setCursor(0,0);

    digitalWrite(LCD_PIN_BACKLIGHT,LOW);
    pinMode(LCD_PIN_BACKLIGHT,OUTPUT);

    Init_I2C_unimaster();
    InitDS3231();
    //DS3231_UpdateClock();

    ltc6804_init();

    /*
        digitalWrite(WIZNET_CS,1);
        pinMode(WIZNET_CS, OUTPUT);

        digitalWrite(5,1);//ltc
        pinMode(5, OUTPUT);

        if (sdcard_init())
        {
            if (system_set_functionmode())
            {
                if (system.funct_mode == FUNCT_MODE_DEBUG)
                    Serial.begin(230400);//Serial.begin(115200);
            }
        }
        lcd_bklight_ON();//lcd_bklight_OFF();

        sdcard_runtest_H10();

        ethernet_init();
    */
    //Serial.begin(230400);//Serial.begin(115200);

    //++--
    InitTCNT1();
    key_initialization();
    TIMSK1 = (1<<OCIE1A);

    lcd_bklight_ON();//lcd_bklight_OFF();

    //--++
    fuelsensor_init();
    jt606_init();

    //+------
    // ethernet_setup();

    while (!ltc6804_job());
    Serial.begin(230400);
}
void loop()
{
    rtc_job();
    ltc6804_job();
    BOARDd();
    kb_processKeyRead_D();

    if (scheduler.sm0 == 0)
        menuBkGround_job();
    else if (scheduler.sm0 == 1)
        main_menu_job();

    lcd_bklight_job();//display off/on


    //analog_input_job();
    //digital_input_job();
    //out_leds_job();
    //rele_job();

    fuelsensor_job();

    //ethernet_uploadData_job();

}

ISR(TIMER1_COMPA_vect)
{
    kb_job();
}


