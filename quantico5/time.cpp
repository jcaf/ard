#include <Arduino.h>
#include "time.h"
#include "ikey.h"
#include "system.h"
#include "kb.h"
#include "rtcc.h"
#include "menu.h"
#include "menu_mainMenu.h"
#include "DS3231.h"
#include "utils.h"

static void mSetTime_print_hh(uint8_t hh);
static void mSetTime_print_mm(uint8_t mm);
static void mSetTime_print_ampm(uint8_t ampm );
static void mSetTime_print(uint8_t set_now);

struct _setTime
{
    int8_t hh;
    int8_t mm;
    int8_t ss;
    int8_t ampm;

    uint8_t set_hms;

    //uint8_t sm0;//x scheduler

    //
    uint8_t sm1;//x blink
    unsigned long last_millis;
    //
    struct _bf
    {
        unsigned print_time:1;
        unsigned blink:1;
        unsigned __a:6;
    } bf;

} setTime;

static void mSetTime_fxKey0(void)//menu
{
}
static void mSetTime_fxKey1(void)//up
{
    if (setTime.set_hms == 0)
    {
        if (++setTime.hh > 12)
        {
            setTime.hh = 1;
        }
    }

    if (setTime.set_hms == 1)
    {
        if (++setTime.mm > 59)
        {
            setTime.mm = 0;
        }
    }

    if (setTime.set_hms == 2)
    {
        if (++setTime.ampm>1)
            setTime.ampm = 0;

    }

    setTime.bf.print_time = 1;
    setTime.bf.blink = 0x00;
    setTime.last_millis = millis();
    setTime.sm1 = 1;

}
static void mSetTime_fxKey2(void)//down
{
    if (setTime.set_hms == 0)
    {
        if (--setTime.hh < 1)
        {
            setTime.hh = 12;
        }
    }

    if (setTime.set_hms == 1)
    {
        if (--setTime.mm < 0)
        {
            setTime.mm = 59;
        }
    }

    if (setTime.set_hms == 2)
    {
        if (--setTime.ampm < 0)
            setTime.ampm = 1;
    }


    setTime.bf.print_time = 1;
    setTime.bf.blink = 0x00;//fijar el param. por 0.5s
    //
    setTime.sm1 = 1;        //empezar nueva temp. despues de ello
    setTime.last_millis = millis();

}
static void mSetTime_fxKey3(void)//enter
{
    mSetTime_print(setTime.set_hms);//print the current param

    if (++setTime.set_hms > 2)
        setTime.set_hms = 0x00;

    //
    setTime.sm1 = 2;
    setTime.bf.blink = 1;

}
static void mSetTime_fxKey4(void)//scape
{
    menu_setSubMenu(MGO_MSETTIME_SAVE_JOB);

    //vscroll.stack_push();
    //vscroll.base_offset_reset();//viene desde main-menu
}

PTRFX_retVOID const mSetTime_fxKey[KB_NUM_KEYS] PROGMEM =
{mSetTime_fxKey0, mSetTime_fxKey1, mSetTime_fxKey2, mSetTime_fxKey3, mSetTime_fxKey4};

void mSetTime_do(void)
{
    uint8_t col;

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mSetTime_fxKey);

        lcd.clear();
        sysPrint_lcd_serialPrintlnPSTR(0,0,PSTR("[SET TIME]"));
        sysPrint_lcd_serialPrintlnPSTR(1,6,PSTR("(hh:mm)"));

        //
        setTime.hh = rtcc.hour;
        setTime.mm = rtcc.min;
        setTime.ss = rtcc.sec;
        setTime.ampm = rtcc.ampm;
        //

        mSetTime_print_hh(setTime.hh);
        lcd.printAtPosc(2,9,":");//blank space
        mSetTime_print_mm(setTime.mm);

        //sysPrint_lcd_serialPrintln(3,8,"AM");
        mSetTime_print_ampm(setTime.ampm);

        setTime.set_hms = 0;

        setTime.sm1 = 1;
        setTime.bf.blink = 0;
        setTime.last_millis = millis();
        //

        setTime.bf.print_time = 1;

        thismenu_sm0 = -1;
    }

    //_____________________________________________________
    if (setTime.bf.print_time)//refresh LCD
    {
        setTime.bf.print_time = 0;

        mSetTime_print(setTime.set_hms);
    }

    //_____________________________________________________
    //blink position
//    if (setTime.sm1 == 0)
//    {
//        setTime.last_millis = millis();
//        setTime.sm1++;
//    }

    if (setTime.sm1 == 1)
    {
        if ( (millis() - setTime.last_millis) > 500)
        {
            setTime.bf.blink = !setTime.bf.blink;
            setTime.sm1++;
        }
    }

    if (setTime.sm1 == 2)
    {
        if (setTime.bf.blink)
        {
            if (setTime.set_hms == 0)
                lcd.printAtPosc(2,7,"  ");//blank space
            else if (setTime.set_hms == 1)
                lcd.printAtPosc(2,10,"  ");//blank space
            else if (setTime.set_hms == 2)
                lcd.printAtPosc(3,8,"  ");//blank space
        }
        else
        {
            mSetTime_print(setTime.set_hms);
        }
        //
        setTime.last_millis = millis();//DIRECT
        //
        setTime.sm1 = 1;//0x00;
    }
}

static void mSetTime_print_hh(uint8_t hh)
{

    uint8_t col= 7;
    if (hh < 10)
        lcd.printAtPosc(2,col++,"0");//blank space

    lcd.printAtPosc(2,col,String(hh));
}

static void mSetTime_print_mm(uint8_t mm)
{
    uint8_t col = 10;

    if (mm < 10)
        lcd.printAtPosc(2,col++,"0");//blank space

    lcd.printAtPosc(2,col,String(mm));
}

static void mSetTime_print_ampm(uint8_t ampm )
{
    if (ampm == 0)
    {
        lcd.printAtPosc(3,8,"AM");
    }
    else
    {
        lcd.printAtPosc(3,8,"PM");
    }

}

static void mSetTime_print(uint8_t set_now)
{
    if (set_now == 0)
        mSetTime_print_hh(setTime.hh);
    else if (set_now == 1)
        mSetTime_print_mm(setTime.mm);
    else if (set_now == 2)
        mSetTime_print_ampm(setTime.ampm);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mSetTime_save_fxKey0(void)
{
}
static inline void mSetTime_save_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mSetTime_save_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void mSetTime_save_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    if (posc == 0)//yes
    {
        uint8_t buff[3];

        //buff[0] = dec2bcd(setTime.ss);
        buff[0] = 0x00;//init from zero
        buff[1] = dec2bcd(setTime.mm);
        buff[2] = (1<<_DS3231_1224_) |  ( setTime.ampm <<_DS3231_AMPM_)  | dec2bcd(setTime.hh) ;

        DS3231_WriteArray2TKRegs(DS3231_SECONDS_REG, buff, 3);
    }

    vscroll.stack_pop();//x el push del main-menu
    menu_setMenu(MGO_MAIN_MENU);
}
static inline void mSetTime_save_fxKey4(void)
{
}

PTRFX_retVOID const mSetTime_save_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mSetTime_save_fxKey0, mSetTime_save_fxKey1, mSetTime_save_fxKey2,
    mSetTime_save_fxKey3, mSetTime_save_fxKey4
};

void mSetTime_save_job(void)
{
    #define MENU_SETTIME_SAVE 2
    String db_mSetTime_save[MENU_SETTIME_SAVE]=
    {
        str_clearPrint("1] Yes",7),
        str_clearPrint("2] No",7)
    };

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mSetTime_save_fxKey);

        vscroll.init(db_mSetTime_save, MENU_SETTIME_SAVE, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        thismenu_sm0 = -1;
    }
    sysPrint_lcdPrintPSTR(0,0, PSTR("[SET TIME]"));
    sysPrint_lcdPrintPSTR(1,STR_CENTER, PSTR("Exit saving Time?"));

    vscroll.set_db(db_mSetTime_save);
    vscroll.job();
    lcd_show_disp(2);//fil 2
}

