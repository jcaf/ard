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

static void setTime_print_hh(uint8_t hh);
static void setTime_print_mm(uint8_t mm);
static void setTime_print_ampm(uint8_t ampm );
static void setTime_print(uint8_t set_now);

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

static void setTime_fxKey0(void)//menu
{
}
static void setTime_fxKey1(void)//up
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
static void setTime_fxKey2(void)//down
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
static void setTime_fxKey3(void)//enter
{
    setTime_print(setTime.set_hms);//print the current param

    if (++setTime.set_hms > 2)
        setTime.set_hms = 0x00;

    //
    setTime.sm1 = 2;
    setTime.bf.blink = 1;

}
static void setTime_fxKey4(void)//scape
{
//    kb_stackPop();
//    taskman_mainMenu.sm0 = 0;
    taskman_setTime.sm0 = 1;
    taskman_setTime.sign = TASKMAN_SIGN_ENTER;
}

PTRFX_retVOID const setTime_fxKey[KB_NUM_KEYS] PROGMEM =
{setTime_fxKey0, setTime_fxKey1, setTime_fxKey2, setTime_fxKey3, setTime_fxKey4};

struct _menuSetTime
{
    int8_t sm0;
} menuSetTime;

void menuSetTime_do(void)
{
    uint8_t col;

    if (menuSetTime.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(setTime_fxKey);

        lcd.clear();
        sysPrint_lcd_serialPrintln(0,0,"[SET TIME]");
        sysPrint_lcd_serialPrintln(1,6,"(hh:mm)");

        //
        setTime.hh = rtcc.hour;
        setTime.mm = rtcc.min;
        setTime.ss = rtcc.sec;
        setTime.ampm = rtcc.ampm;
        //

        setTime_print_hh(setTime.hh);
        lcd.printAtPosc(2,9,":");//blank space
        setTime_print_mm(setTime.mm);

        //sysPrint_lcd_serialPrintln(3,8,"AM");
        setTime_print_ampm(setTime.ampm);

        setTime.set_hms = 0;

        setTime.sm1 = 1;
        setTime.bf.blink = 0;
        setTime.last_millis = millis();
        //

        setTime.bf.print_time = 1;

        menuSetTime.sm0 = -1;
    }

    //_____________________________________________________
    if (setTime.bf.print_time)//refresh LCD
    {
        setTime.bf.print_time = 0;

        setTime_print(setTime.set_hms);
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
            setTime_print(setTime.set_hms);
        }
        //
        setTime.last_millis = millis();//DIRECT
        //
        setTime.sm1 = 1;//0x00;

    }

}


static void setTime_print_hh(uint8_t hh)
{

    uint8_t col= 7;
    if (hh < 10)
        lcd.printAtPosc(2,col++,"0");//blank space

    lcd.printAtPosc(2,col,String(hh));
}

static void setTime_print_mm(uint8_t mm)
{
    uint8_t col = 10;

    if (mm < 10)
        lcd.printAtPosc(2,col++,"0");//blank space

    lcd.printAtPosc(2,col,String(mm));
}

static void setTime_print_ampm(uint8_t ampm )
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

static void setTime_print(uint8_t set_now)
{
    if (set_now == 0)
        setTime_print_hh(setTime.hh);
    else if (set_now == 1)
        setTime_print_mm(setTime.mm);
    else if (set_now == 2)
        setTime_print_ampm(setTime.ampm);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//VScroll vscroll;

static inline void menuSetTime_save_fxKey0(void)
{
}
static inline void menuSetTime_save_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void menuSetTime_save_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void menuSetTime_save_fxKey3(void)
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

    kb_stackPop();
    kb_stackPop();

    taskman_mainMenu.sm0 = 0;   //MAIN - MENU

    //
    taskman_setTime.sm0 = 0;
    taskman_setTime.sign = TASKMAN_SIGN_ENTER;
    //

}
static inline void menuSetTime_save_fxKey4(void)
{

}

PTRFX_retVOID const menuSetTime_save_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuSetTime_save_fxKey0, menuSetTime_save_fxKey1, menuSetTime_save_fxKey2,
    menuSetTime_save_fxKey3, menuSetTime_save_fxKey4
};


struct _menuSetTime_save
{
    int8_t sm0;
} menuSetTime_save;

static void menuSetTime_save_upTimeDbData(String *db);

static void menuSetTime_save_job(void)
{
    #define MENU_SETTIME_SAVE 2
    //static
    String db_menuSetTime_save[MENU_SETTIME_SAVE]=
    {
        str_clearPrint("1] Yes",7),
        str_clearPrint("2] No",7)
    };

    if (menuSetTime_save.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuSetTime_save_fxKey);

        //menuSetTime_save_upTimeDbData(db_menuSetTime_save);
        vscroll.init(db_menuSetTime_save, MENU_SETTIME_SAVE, dispShowBuff, 2, FEAT_MARK_ON);

        menuSetTime_save.sm0 = -1;
    }
    sysPrint_lcdPrintPSTR(0,0, PSTR("[SET TIME]"));
    sysPrint_lcdPrintPSTR(1,STR_CENTER, PSTR("Exit saving Time?"));

    vscroll.set_db(db_menuSetTime_save);
    vscroll.job();
    lcd_show_disp(2);//fil 2

    //menuSetTime_save_upTimeDbData(db_menuSetTime_save);
}

//static void menuSetTime_save_upTimeDbData(String *db)
//{
//    db[0] = str_clearPrint("1] Yes",7);
//    db[1] = str_clearPrint("2] No",7);
//}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct _taskman_setTime taskman_setTime;

//struct _taskman_setTime
//{
//    int8_t sm0;
//    int8_t sign;
//    int8_t reInitJob;
//
//} taskman_setTime;

void mainMenu_setTime_job(void)
{
    if (taskman_setTime.sign == TASKMAN_SIGN_ENTER)
    {
        taskman_setTime.reInitJob = 1;
    }

    if (taskman_setTime.sm0 == 0)
    {
        if (taskman_setTime.reInitJob)
            menuSetTime.sm0 = 0;

        menuSetTime_do();//Do
    }

    if (taskman_setTime.sm0 == 1)
    {
        if (taskman_setTime.reInitJob)
            menuSetTime_save.sm0 = 0;

        menuSetTime_save_job();//saving?
    }

    //
    if(taskman_setTime.reInitJob == 1)
        taskman_setTime.reInitJob = 0;

    if (taskman_setTime.sign == TASKMAN_SIGN_ENTER)
        taskman_setTime.sign = TASKMAN_SIGN_CLEAR;
    //
}


