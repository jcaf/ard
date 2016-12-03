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

#include "fuelsensor.h"


static inline void mInput_menu_fxKey0(void)
{
}
static inline void mInput_menu_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mInput_menu_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mInput_menu_fxKey3(void)
{
    vscroll.stack_push();
    vscroll.base_offset_reset();

    switch (vscroll.get_markPosc())
    {
        case 0: menu_setSubMenu(MGO_MINPUT_DIGITALINPUT_JOB);break;
        //case 1: menu_setSubMenu(MGO_MINPUT_DIGITALINPUT_JOB);break;
        case 2: menu_setSubMenu(MGO_MINPUT_FUELSENSOR_JOB);break;
    }
}
static inline void mInput_menu_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setMenu(MGO_MAIN_MENU);
}

PTRFX_retVOID const mInput_menu_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mInput_menu_fxKey0, mInput_menu_fxKey1, mInput_menu_fxKey2,
    mInput_menu_fxKey3, mInput_menu_fxKey4
};


void mInput_menu_job(void)
{
    #define MENU_INPUT_SUMMARY_NUMOPT 3
    String db_mInput_menu[MENU_INPUT_SUMMARY_NUMOPT]=
    {
        str_clearPrint("1] Digital Input",2),
        str_clearPrint("2] Analog Input",2),
        str_clearPrint("3] Fuel Sensor",2),
    };

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mInput_menu_fxKey);
        vscroll.init(db_mInput_menu, MENU_INPUT_SUMMARY_NUMOPT, dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        thismenu_sm0++;
    }

    if (thismenu_sm0 == 1)
    {
        lcd.setCursor(0, 0);
        lcd.print( str_clearPrint("[INPUTS]",0));

        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mInput_menu);
    vscroll.job();
    lcd_show_disp();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mInput_digitalInput_fxKey0(void)
{
}
static inline void mInput_digitalInput_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mInput_digitalInput_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mInput_digitalInput_fxKey3(void)
{
}
static inline void mInput_digitalInput_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setSubMenu(MGO_MINPUT_MENU_JOB);
}

PTRFX_retVOID const mInput_digitalInput_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mInput_digitalInput_fxKey0, mInput_digitalInput_fxKey1, mInput_digitalInput_fxKey2,
    mInput_digitalInput_fxKey3, mInput_digitalInput_fxKey4
};

static void mInput_digitalInput_updateDbData(String *db)
{
    String str;

    for (uint8_t i=0; i<NUM_DIG_INPUT_MAX; i++ )
    {
        str = "Dig-Input "+ String(i+1) + ": " + String(digital_input[i]);
        db[i] = str_clearPrint(str,3);
    }
}

//static
void mInput_digitalInput_job(void)
{
    String db_mInput_digitalInput[NUM_DIG_INPUT_MAX];

    mInput_digitalInput_updateDbData(db_mInput_digitalInput);

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mInput_digitalInput_fxKey);
        vscroll.init(db_mInput_digitalInput, NUM_DIG_INPUT_MAX, dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        thismenu_sm0 = -1;
    }

    lcd.setCursor(0, 0);
    lcd.print( str_clearPrint("[DIGITAL INPUTS]",0));

    vscroll.set_db(db_mInput_digitalInput);
    vscroll.job();
    lcd_show_disp();
}

/////////////////////////////////////////////////////////////
static inline void mInput_fuelSensor_fxKey0(void)
{
}
static inline void mInput_fuelSensor_fxKey1(void)
{
}
static inline void mInput_fuelSensor_fxKey2(void)
{
}
static inline void mInput_fuelSensor_fxKey3(void)
{
}
static inline void mInput_fuelSensor_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setSubMenu(MGO_MINPUT_MENU_JOB);
}
PTRFX_retVOID const mInput_fuelSensor_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mInput_fuelSensor_fxKey0, mInput_fuelSensor_fxKey1, mInput_fuelSensor_fxKey2,
    mInput_fuelSensor_fxKey3, mInput_fuelSensor_fxKey4
};
void mInput_fuelSensor_job(void)
{
    if (thismenu_sm0 == 0)//siempre SOLO cuando se entra!!!!
    {
        kb_change_keyDo_pgm(mInput_fuelSensor_fxKey);

        lcd.clear();
        sysPrint_lcdPrintPSTR(LCD_ROW_0, 0, PSTR("[FUEL SENSOR]"));

        thismenu_sm0++;
    }

    sysPrint_lcdPrint(LCD_ROW_1, 0, fuelsensor_reading());
}

