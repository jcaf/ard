#include <Arduino.h>
#include "system.h"
#include "vscroll.h"
#include "lcd_fxaux.h"
#include "ikey.h"
#include "kb.h"

#include "digital_input.h"
#include "menu.h"
#include "menu_mainMenu.h"
#include "menu_inputs.h"

struct _taskman_input taskman_input;

static void menuInputSummary_job(void);
static void menu_digitalinput_job(void);
//static void menu_analoginput_job(void);
//static void menu_fuelsensor_job(void);

void mainMenu_inputs_job(void)
{

    if (taskman_input.sign  == TASKMAN_SIGN_ENTER)
    {
        taskman_input.reInitJob = 1;
    }

    //summary
    if (taskman_input.sm0 == 0)
    {
        if (taskman_input.reInitJob)
            menuInputSummary.sm0 = 0;

        menuInputSummary_job();
    }
    //choice option :
    if (taskman_input.sm0 == 1)
    {
        if (taskman_input.reInitJob)
            menu_digitalinput.sm0 = 0;

        menu_digitalinput_job();
    }
    if (taskman_input.sm0 == 2)
    {
        //menu_analoginput_job();
    }
    if (taskman_input.sm0 == 3)
    {
        //menu_fuelsensor_job();
    }


    if(taskman_input.reInitJob == 1)
        taskman_input.reInitJob = 0;

    if(taskman_input.sign != TASKMAN_SIGN_CLEAR)
        taskman_input.sign = TASKMAN_SIGN_CLEAR;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_INPUT_SUMMARY_NUMOPT 3
//
String db_menuInputSummary[MENU_INPUT_SUMMARY_NUMOPT ];
VScroll VS_menuInputSummary;

static inline void menuInputSummary_fxKey0(void)
{
}
static inline void menuInputSummary_fxKey1(void)
{
    VS_menuInputSummary.sign_up();
}
static inline void menuInputSummary_fxKey2(void)
{
    VS_menuInputSummary.sign_down();
}
static inline void menuInputSummary_fxKey3(void)
{
    uint8_t posc = VS_menuInputSummary.get_markPosc() +1;
    if (posc == 1)
    {

        taskman_input.sm0 = posc;
        taskman_input.sign  = TASKMAN_SIGN_ENTER;
    }

}
static inline void menuInputSummary_fxKey4(void)
{
    kb_stackPop();

    taskman_mainMenu.sm0 = 0;

    //menuInputSummary.sm0 = 1;
//    taskman.sign = TASKMAN_SIGN_EXIT;
}

PTRFX_retVOID const menuInputSummary_fxKey[KB_NUM_KEYS] PROGMEM =
{menuInputSummary_fxKey0, menuInputSummary_fxKey1, menuInputSummary_fxKey2,
 menuInputSummary_fxKey3, menuInputSummary_fxKey4};

struct _menuInputSummary menuInputSummary;

static void menuInputSummary_job(void)
{
    //static
    String db_menuInputSummary[MENU_INPUT_SUMMARY_NUMOPT]=
    {
        str_clearPrint("1] Digital Input",2),
        str_clearPrint("2] Analog Input",2),
        str_clearPrint("3] Fuel Sensor",2),
    };

    if (menuInputSummary.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuInputSummary_fxKey);


        VS_menuInputSummary.init(db_menuInputSummary, MENU_INPUT_SUMMARY_NUMOPT , dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON);


        lcd.setCursor(0, 0);lcd.print( str_clearPrint("[INPUTS]",0));

        menuInputSummary.sm0 = -1;
    }

    VS_menuInputSummary.set_db(db_menuInputSummary);
    VS_menuInputSummary.job();
    lcd_show_disp();

    //menuInputSummary_updateDbData(db_menuInputSummary);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
VScroll VS_menu_digitalinput;

static inline void menu_digitalinput_fxKey0(void)
{

}
static inline void menu_digitalinput_fxKey1(void)
{
    VS_menu_digitalinput.sign_up();
}
static inline void menu_digitalinput_fxKey2(void)
{
    VS_menu_digitalinput.sign_down();
}
static inline void menu_digitalinput_fxKey3(void)
{

}
static inline void menu_digitalinput_fxKey4(void)
{
    kb_stackPop();
    taskman_input.sm0 = 0;

    //menu_digitalinput.sm0 = 1;
//    taskman.sign = TASKMAN_SIGN_EXIT;
}

PTRFX_retVOID const menu_digitalinput_fxKey[KB_NUM_KEYS] PROGMEM =
{menu_digitalinput_fxKey0, menu_digitalinput_fxKey1, menu_digitalinput_fxKey2,
 menu_digitalinput_fxKey3, menu_digitalinput_fxKey4};

struct _menu_digitalinput menu_digitalinput;

static void menu_digitalinput_updateDbData(String *db)
{
    String str;

    for (uint8_t i=0; i<NUM_DIG_INPUT_MAX; i++ )
    {
        str = "Dig-Input "+ String(i+1) + ": " + String(digital_input[i]);
        db[i] = str_clearPrint(str,3);
    }
}

static void menu_digitalinput_job(void)
{
    //static
    String db_menu_digitalinput[NUM_DIG_INPUT_MAX];

    menu_digitalinput_updateDbData(db_menu_digitalinput);

    if (menu_digitalinput.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menu_digitalinput_fxKey);


        //menu_digitalinput_updateDbData(db_menu_digitalinput);
        VS_menu_digitalinput.init(db_menu_digitalinput, NUM_DIG_INPUT_MAX , dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON);

        //lcd.setCursor(0, 0);lcd.print( str_clearPrint("[DIGITAL INPUTS]",0));
        menu_digitalinput.sm0 = -1;
    }

    lcd.setCursor(0, 0);lcd.print( str_clearPrint("[DIGITAL INPUTS]",0));

    VS_menu_digitalinput.set_db(db_menu_digitalinput);
    VS_menu_digitalinput.job();
    lcd_show_disp();
}


