#include <Arduino.h>
#include "date.h"
#include "ikey.h"
#include "system.h"
#include "kb.h"
#include "rtcc.h"

#include "menu.h"
#include "menu_mainMenu.h"



#include "DS3231.h"
#include "utils.h"

static void menuSetDate_save_job(void);


//const char dayOfWeek_str[7][10]= {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

static void setDate_print_date(uint8_t date);
static void setDate_print_month(uint8_t month);
static void setDate_print_year (uint8_t year);

static void setDate_print(uint8_t currentParam);

static void setDate_print_blankspace(uint8_t currentParam);

//optional
static void dayOfWeek(void);

struct _setDate
{
    int8_t date;
    int8_t month;
    int8_t year;
    int8_t dayOfWeek;

    uint8_t currentParam;

    //uint8_t sm0;//x scheduler

    //--> encerrar dentro de la estructura
    uint8_t sm1;//x blink
    unsigned long last_millis;
    //
    struct _bf
    {
        unsigned print_date:1;
        unsigned blink:1;
        unsigned __a:6;
    } bf;

    //optional
    struct _xyprint
    {
        struct _date
        {
            uint8_t fil;
            uint8_t col;
        } date;

    } xyprint;

} setDate;

inline static uint8_t isLeap(uint16_t year)
{
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

static uint8_t setDate_getNumberdateOfMonth(uint8_t month)
{
    uint8_t datesOfmonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    if ( isLeap( 2000 + setDate.year ) )//update to 29 dates ?
        datesOfmonth[1] = 29;

    return datesOfmonth[month-1];
}

static void setDate_fixdateAccordingNumberdateOfMonth(void)
{
    uint8_t NumberdateOfMonth = setDate_getNumberdateOfMonth(setDate.month);
    if (setDate.date > NumberdateOfMonth)
        setDate.date = NumberdateOfMonth;

    setDate_print_date(setDate.date);
}

static void setDate_fxKey0(void)//menu
{
}

static void setDate_fxKey1(void)//up
{

    if (setDate.currentParam == 0)
    {
        if (++setDate.date > setDate_getNumberdateOfMonth(setDate.month))
            setDate.date = 1;
    }

    if (setDate.currentParam == 1)
    {
        if (++setDate.month > 12)
            setDate.month = 1;

        setDate_fixdateAccordingNumberdateOfMonth();
    }

    if (setDate.currentParam == 2)
    {
        if (++setDate.year > 99)
            setDate.year = 0;

        setDate_fixdateAccordingNumberdateOfMonth();
    }

    dayOfWeek();

    setDate.bf.print_date = 1;
    setDate.bf.blink = 0x00;
    setDate.last_millis = millis();
    setDate.sm1 = 1;
}

static void setDate_fxKey2(void)//down
{
    if (setDate.currentParam == 0)
    {
        if (--setDate.date < 1)
            setDate.date = setDate_getNumberdateOfMonth(setDate.month);
    }

    if (setDate.currentParam == 1)
    {
        if (--setDate.month < 1)
            setDate.month = 12;

        setDate_fixdateAccordingNumberdateOfMonth();
    }

    /**<  DS3231: Year, with Leap-Year Compensation Valid Up to 2100, (00->99)*/
    if (setDate.currentParam == 2)
    {
        if (--setDate.year < 0)
            setDate.year = 99;

        setDate_fixdateAccordingNumberdateOfMonth();
    }

    dayOfWeek();

    setDate.bf.print_date = 1;
    setDate.bf.blink = 0x00;//fijar el param. por 0.5s
    //
    setDate.sm1 = 1;        //empezar nueva temp. despues de ello
    setDate.last_millis = millis();

}
static void setDate_fxKey3(void)//enter
{
    setDate_print(setDate.currentParam);//print the current param

    if (++setDate.currentParam > 2)
        setDate.currentParam = 0x00;

    //
    setDate.sm1 = 2;
    setDate.bf.blink = 1;

}
static void setDate_fxKey4(void)//scape
{
    //kb_stackPop();
    //taskman_mainMenu.sm0 = 0;

    taskman_setDate.sm0 = 1;
    taskman_setDate.sign = TASKMAN_SIGN_ENTER;

}

PTRFX_retVOID const setDate_fxKey[KB_NUM_KEYS] PROGMEM =
{setDate_fxKey0, setDate_fxKey1, setDate_fxKey2, setDate_fxKey3, setDate_fxKey4};


struct _menuSetDate menuSetDate;

/////////////////////////////////////////////////////////////////
void menuSetDate_do(void)
{
    if (menuSetDate.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(setDate_fxKey);

        lcd.clear();

        sysPrint_lcd_serialPrintln(0,0,"[SET DATE]");

        String date_format = "(dd/mm/yyyy)";
        sysPrint_lcd_serialPrintln(1,STR_CENTER,date_format);

        //
        setDate.xyprint.date.fil = 2;
        setDate.xyprint.date.col= str_center(date_format) +1;
        //

        uint8_t col = setDate.xyprint.date.col;

        //
        setDate.date = rtcc.date;// 1;
        setDate.month = rtcc.month;  //1;
        setDate.year = rtcc.year;//00;
        setDate.dayOfWeek = rtcc.day;/**< DS3231 day of week: range 1-7 */
        //

        //
        setDate_print_date(setDate.date);
        col+=2;
        lcd.printAtPosc(2,col,"/");
        setDate_print_month(setDate.month);
        col+=3;
        lcd.printAtPosc(2,col,"/");
        setDate_print_year(setDate.year);

        //
        dayOfWeek();
        /**< DS3231 day of week: range 1-7 */
//sysPrint_lcdPrint(3,STR_CENTER, String(dayOfWeek_str[setDate.dayOfWeek-1]));
        //

        setDate.currentParam = 0;

        setDate.sm1 = 1;
        setDate.bf.blink = 0;
        setDate.last_millis = millis();
        //

        setDate.bf.print_date = 1;

        menuSetDate.sm0 = -1;
    }

    //_____________________________________________________
    if (setDate.bf.print_date)//refresh LCD
    {
        setDate.bf.print_date = 0;

        setDate_print(setDate.currentParam);
    }

    //_____________________________________________________
    //blink position
//    if (setDate.sm1 == 0)
//    {
//        setDate.last_millis = millis();
//        setDate.sm1++;
//    }

    if (setDate.sm1 == 1)
    {
        if ( (millis() - setDate.last_millis) > 500)
        {
            setDate.bf.blink = !setDate.bf.blink;
            setDate.sm1++;
        }
    }

    if (setDate.sm1 == 2)
    {
        if (setDate.bf.blink)
        {
            setDate_print_blankspace(setDate.currentParam);

        }
        else
        {
            setDate_print(setDate.currentParam);
        }
        //
        setDate.last_millis = millis();//DIRECT
        //
        setDate.sm1 = 1;//0x00;

    }

}


static void setDate_print_date(uint8_t date)
{
    uint8_t col= setDate.xyprint.date.col;

    if (date < 10)
        lcd.printAtPosc(setDate.xyprint.date.fil, col++,"0");//blank space

    lcd.printAtPosc(setDate.xyprint.date.fil, col, String(date));
}

static void setDate_print_month(uint8_t month)
{
    uint8_t col= setDate.xyprint.date.col +3;

    if (month < 10)
        lcd.printAtPosc(setDate.xyprint.date.fil, col++,"0");//blank space

    lcd.printAtPosc(setDate.xyprint.date.fil, col, String(month));
}


static void setDate_print_year(uint8_t year )
{
    /**<  DS3231: Year, with Leap-Year Compensation Valid Up to 2100*/

    uint8_t col= setDate.xyprint.date.col +6;

    lcd.printAtPosc(setDate.xyprint.date.fil, col,"20");

//    if (year <100)
//        lcd.printAtPosc(setDate.xyprint.date.fil, ++col,"0");

    col +=2;
    if (year <10)
        lcd.printAtPosc(setDate.xyprint.date.fil, col++,"0");

    lcd.printAtPosc(setDate.xyprint.date.fil, col, String(year));
}

static void setDate_print(uint8_t currentParam)
{
    if (currentParam == 0)
        setDate_print_date(setDate.date);
    else if (currentParam == 1)
        setDate_print_month(setDate.month);
    else if (currentParam == 2)
        setDate_print_year(setDate.year);

}
static void setDate_print_blankspace(uint8_t currentParam)
{
    uint8_t col= setDate.xyprint.date.col;

    if (currentParam == 0)
        lcd.printAtPosc(setDate.xyprint.date.fil, col,"  ");//blank space
    else if (currentParam == 1)
        lcd.printAtPosc(setDate.xyprint.date.fil, col+3,"  ");//blank space
    else if (currentParam == 2)
        lcd.printAtPosc(setDate.xyprint.date.fil, col+6,"    ");//blank space
}


//Optional
static uint8_t get_dayOfWeek(void)
{
    // The month table used in date of week calculation
    // January 6 for leap year, February 2 for leap year
    const unsigned char monthsTable [12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
    enum
    {
        January, February, March, April, May, June, July, August, September, October, November, December
    };
    //char dayOfWeek_str[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    //char dayOfWeek_str[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    int temp = 0;
    temp += 6;
    temp += setDate.year;
    temp += (int) (setDate.year >> 2);
    temp += monthsTable[setDate.month - 1]; //Esto compatibiliza el enum{}
    temp += setDate.date;
    // leap year correction
    //if (!(setDate.year % 4))
    if ( isLeap( 2000 + setDate.year ) )
    {
        if ( (setDate.month-1) == January)
            temp += 6;
        if ( (setDate.month-1) == February)
            temp -= 1;
    }
    return (temp % 7);

}
static void dayOfWeek(void)
{
    const char dayOfWeek_str[7][10]= {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    setDate.dayOfWeek = get_dayOfWeek();

    sysPrint_lcdPrint(3,STR_CENTER, String(dayOfWeek_str[setDate.dayOfWeek]));
}

/////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

//VScroll vscroll;

static inline void menuSetDate_save_fxKey0(void)
{
}
static inline void menuSetDate_save_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void menuSetDate_save_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void menuSetDate_save_fxKey3(void)
{

    uint8_t posc = vscroll.get_markPosc();

    if (posc == 0)//yes
    {
        uint8_t buff[4];

        buff[0] = dec2bcd(setDate.dayOfWeek +1);//hacer dayOfWeek compatible con DS3231 (Sun=1, Mon-2...)
        buff[1] = dec2bcd(setDate.date);
        buff[2] = dec2bcd(setDate.month);
        buff[3] = dec2bcd(setDate.year);

        DS3231_WriteArray2TKRegs(DS3231_DAY_REG, buff, 4);
    }

    kb_stackPop();
    kb_stackPop();

    taskman_mainMenu.sm0 = 0;   //MAIN - MENU

    //
    taskman_setDate.sm0 = 0;
    taskman_setDate.sign = TASKMAN_SIGN_ENTER;
    //

}
static inline void menuSetDate_save_fxKey4(void)
{

}

PTRFX_retVOID const menuSetDate_save_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuSetDate_save_fxKey0, menuSetDate_save_fxKey1, menuSetDate_save_fxKey2,
    menuSetDate_save_fxKey3, menuSetDate_save_fxKey4
};

struct _menuSetDate_save menuSetDate_save;;

static void menuSetDate_save_updateDbData(String *db);

static void menuSetDate_save_job(void)
{
    #define MENU_SETDATE_SAVE 2
    static String db_menuSetDate_save[MENU_SETDATE_SAVE]=
    {
        str_clearPrint("1] Yes",7),
        str_clearPrint("2] No",7)
    };

    if (menuSetDate_save.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuSetDate_save_fxKey);

        //menuSetDate_save_updateDbData(db_menuSetDate_save);
        vscroll.init(db_menuSetDate_save, MENU_SETDATE_SAVE, dispShowBuff, 2, FEAT_MARK_ON);

        menuSetDate_save.sm0 = -1;
    }
    sysPrint_lcdPrintPSTR(0,0, PSTR("[SET TIME]"));
    sysPrint_lcdPrintPSTR(1,STR_CENTER, PSTR("Exit saving date?"));

    vscroll.set_db(db_menuSetDate_save);
    vscroll.job();
    lcd_show_disp(2);//fil 2

    //menuSetDate_save_updateDbData(db_menuSetDate_save);
}

//static void menuSetDate_save_updateDbData(String *db)
//{
//    db[0] = str_clearPrint("1] Yes",7);
//    db[1] = str_clearPrint("2] No",7);
//}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct _taskman_setDate taskman_setDate;

void mainMenu_setDate_job(void)
{
    if (taskman_setDate.sign == TASKMAN_SIGN_ENTER)
    {
        taskman_setDate.reInitJob = 1;
    }

    if (taskman_setDate.sm0 == 0)
    {
        if (taskman_setDate.reInitJob)
            menuSetDate.sm0 = 0;

        menuSetDate_do();
    }

    if (taskman_setDate.sm0 == 1)
    {
        if (taskman_setDate.reInitJob)
            menuSetDate_save.sm0 = 0;

        menuSetDate_save_job();
    }

    //
    if(taskman_setDate.reInitJob == 1)
        taskman_setDate.reInitJob = 0;

    if (taskman_setDate.sign == TASKMAN_SIGN_ENTER)
        taskman_setDate.sign = TASKMAN_SIGN_CLEAR;
    //
}

