#include <Arduino.h>
#include "system.h"
#include "vscroll.h"
#include "lcd_fxaux.h"
#include "ikey.h"
#include "kb.h"
#include "board.h"
#include "menu.h"

static inline void mainMenuSummary_fxKey0(void)
{
}
static inline void mainMenuSummary_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mainMenuSummary_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mainMenuSummary_fxKey3(void)
{
    menu_setMenu(vscroll.get_markPosc()+1); //men=[1...8]//men = vscroll.get_markPosc()+1;//pos 0 is for bkground
    menu_setSubMenu(0);                      //submen = 0;
    //
    vscroll.stack_push();
    vscroll.base_offset_reset();
}
static inline void mainMenuSummary_fxKey4(void)
{
    menu_setMenu(MGO_MBKGROUND);//men = 0;
    menu_setSubMenu(MGO_MBKGROUND_SHOWSTACKSUMMARY_JOB);//submen=0
    vscroll.base_offset_reset();
    //
    boardD.AchangeInBoard =1;
    lcd.clear();
}

PTRFX_retVOID const mainMenuSummary_fxKey[KB_NUM_KEYS] PROGMEM =
{mainMenuSummary_fxKey0, mainMenuSummary_fxKey1, mainMenuSummary_fxKey2, mainMenuSummary_fxKey3, mainMenuSummary_fxKey4};


void mainMenuSummary_job(void)
{
    String db_mainMenuSummary[MAIN_MENU_NUMMAX]=
    {
        str_clearPrint("1] INPUTS", 4),
        str_clearPrint("2] OUTPUTS", 4),
        str_clearPrint("3] SYS ERROR", 4),
        str_clearPrint("4] INFORMATION", 4),
        str_clearPrint("5] CONTACT", 4),
        str_clearPrint("6] SET TIME", 4),
        str_clearPrint("7] SET DATE", 4),
        str_clearPrint("8] FUEL SENSOR", 4),
    };

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mainMenuSummary_fxKey);

        vscroll.init(db_mainMenuSummary, MAIN_MENU_NUMMAX, dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        lcd.setCursor(0, 0);
        lcd.print( str_clearPrint("[MAIN MENU]",0));

        thismenu_sm0 = -1;
    }
    vscroll.set_db(db_mainMenuSummary);
    vscroll.job();
    lcd_show_disp();
}

