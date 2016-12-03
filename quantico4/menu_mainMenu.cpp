#include <Arduino.h>
#include "system.h"
#include "vscroll.h"
#include "lcd_fxaux.h"
#include "ikey.h"
#include "kb.h"
#include "board.h"

#include "date.h"
#include "time.h"

#include "menu.h"
#include "menu_mainMenu.h"
#include "menu_bkground.h"//x SIGN_ENTER/EXIT/CLEAR
#include "menu_inputs.h"
#include "menu_fuelSensor.h"


void mainMenuSummary_job(void);

struct _taskman_mainMenu taskman_mainMenu;

void main_menu_job(void)
{
    if (taskman_mainMenu.sm0 == 0)
    {
        mainMenuSummary_job();
    }
    if (taskman_mainMenu.sm0 == 1)
    {
        mainMenu_inputs_job();
    }
    if (taskman_mainMenu.sm0 == 2)
    {
        //mainMenu_outputs_job();
    }
    if (taskman_mainMenu.sm0 == 3)
    {
        //mainMenu_sysError_job();
    }
    if (taskman_mainMenu.sm0 == 4)
    {
        //mainMenu_information_job();
    }
    if (taskman_mainMenu.sm0 == 5)
    {
        //mainMenu_contact_job();
    }

    if (taskman_mainMenu.sm0 == 6)
    {
        mainMenu_setTime_job();

    }
    if (taskman_mainMenu.sm0 == 7)
    {
        mainMenu_setDate_job();
    }

    if (taskman_mainMenu.sm0 == 8)
    {
        mainMenu_fuelSensor_job();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
VScroll VS_mainMenuSummary;

static inline void mainMenuSummary_fxKey0(void)
{
}
static inline void mainMenuSummary_fxKey1(void)
{
    VS_mainMenuSummary.sign_up();
}
static inline void mainMenuSummary_fxKey2(void)
{
    VS_mainMenuSummary.sign_down();
}
static inline void mainMenuSummary_fxKey3(void)
{
    uint8_t posc = VS_mainMenuSummary.get_markPosc() +1;
    //no cuenta la primera opcion, pues el background q siempre se ejecuta
    taskman_mainMenu.sm0 = posc;

    switch (posc)
    {
        case 1:
            //taskman.sign = TASKMAN_SIGN_ENTER;
            taskman_input.sign  = TASKMAN_SIGN_ENTER;
            break;

        case 2:
            break;

        case 3:
            break;

        case 4:
            break;

        case 5:
            break;

        case 6:
            taskman_setTime.sign = TASKMAN_SIGN_ENTER;
            break;
        case 7:
            taskman_setDate.sign = TASKMAN_SIGN_ENTER;
            break;

        case 8:
            taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;
            break;

        default:
            break;
    }

}
static inline void mainMenuSummary_fxKey4(void)
{
    kb_stackPop();
    scheduler.sm0 = SCHEDULER_BK_GROUND;

    boardD.set_text = 1;//show texts again
    //mainMenuSummary.sm0 = 1;  //leave for next show-MENU
}

PTRFX_retVOID const mainMenuSummary_fxKey[KB_NUM_KEYS] PROGMEM =
{mainMenuSummary_fxKey0, mainMenuSummary_fxKey1, mainMenuSummary_fxKey2, mainMenuSummary_fxKey3, mainMenuSummary_fxKey4};

struct _mainMenuSummary mainMenuSummary;
static void mainMenuSummary_updateDbData(String *db);

void mainMenuSummary_job(void)
{
#define DB_mainMenuSummary_NUM_OPT 8//NUM OPT x main menu
    static String db_mainMenuSummary[DB_mainMenuSummary_NUM_OPT];//max

    if (mainMenuSummary.sm0 == 0)//creation object
    {
        mainMenuSummary_updateDbData(db_mainMenuSummary);
        VS_mainMenuSummary.init(db_mainMenuSummary, DB_mainMenuSummary_NUM_OPT, dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON);

        kb_stackPush();
        kb_change_keyDo_pgm(mainMenuSummary_fxKey);


        mainMenuSummary.sm0 = -1;
    }

    lcd.setCursor(0, 0);
    lcd.print( str_clearPrint("[MAIN MENU]",0));

    VS_mainMenuSummary.job();
    lcd_show_disp();
}

static void mainMenuSummary_updateDbData(String *db)
{
    db[0] = str_clearPrint("1] INPUTS", 4);
    db[1] = str_clearPrint("2] OUTPUTS", 4);
    db[2] = str_clearPrint("3] SYS ERROR", 4);
    db[3] = str_clearPrint("4] INFORMATION", 4);
    db[4] = str_clearPrint("5] CONTACT", 4);
    db[5] = str_clearPrint("6] SET TIME", 4);
    db[6] = str_clearPrint("7] SET DATE", 4);
    db[7] = str_clearPrint("8] FUEL SENSOR", 4);
}
