/****************************************************************************
* Project: Battery System
*
* 2016-05-11-19.38
* jcaf @ jcafOpenSuse
*
* Copyright 2016 Juan Carlos Agüero Flores
*
* Licensed under the Apache License, Version 2.0 (the "License"));
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
****************************************************************************/
#include <Arduino.h>
#include "system.h"
#include "utils.h"

#include "vscroll.h"
#include "lcd_fxaux.h"
#include "ikey.h"
#include "kb.h"

#include "menu.h"
#include "menu_fuelSensor.h"
#include "fuelsensor.h"
#include "fuelsensor_jt606x.h"
#include "menu_mainMenu.h"

#include "menu_fuelSensor_fuelTank.h"
#include "menu_fuelSensor_calibration.h"
#include "menu_fuelSensor_irregularVolume.h"
#include "menu_fuelSensor_modelOfSensor.h"
#include "menu_fuelSensor_outputTypeUnits.h"
#include "menu_fuelSensor_regularVolume.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mFuelSensor_warning_fxKey0(void)
{
}
static inline void mFuelSensor_warning_fxKey1(void)
{
    if (--thismenu_sm1 < 0)
        thismenu_sm1 = 0;
    else
        thismenu_sm0 = 1;
}
static inline void mFuelSensor_warning_fxKey2(void)
{
    if (++thismenu_sm1 > 1)
    {
        thismenu_sm1 = 0;

        menu_setSubMenu(MGO_MFUELSENSOR_MENU_JOB);
    }
    else
        thismenu_sm0 = 1;
}
static inline void mFuelSensor_warning_fxKey3(void)
{
}
static inline void mFuelSensor_warning_fxKey4(void)
{
}

PTRFX_retVOID const mFuelSensor_warning_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_warning_fxKey0, mFuelSensor_warning_fxKey1, mFuelSensor_warning_fxKey2,
    mFuelSensor_warning_fxKey3, mFuelSensor_warning_fxKey4
};

void mFuelSensor_warning_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_warning_fxKey);

        thismenu_sm1 = 0;
        thismenu_sm0++;
    }

    if (thismenu_sm0 == 1)
    {
        if (thismenu_sm1 == 0)
        {
            sysPrint_lcdPrintPSTR(0, STR_CENTER, PSTR("[WARNING]"));
            sysPrint_lcdPrintPSTR(1, STR_CENTER, PSTR("This sensor use"));
            sysPrint_lcdPrintPSTR(2, STR_CENTER, PSTR("diesel, biodisel,"));
            sysPrint_lcdPrintPSTR(3, STR_CENTER, PSTR("kerosene, gasoline"));
        }

        else if (thismenu_sm1 == 1)
        {
            sysPrint_lcdPrintPSTR(0, STR_CENTER, PSTR("Do not measure"));
            sysPrint_lcdPrintPSTR(1, STR_CENTER, PSTR("conductive material"));
            sysPrint_lcdPrintPSTR(2, STR_CENTER, PSTR("like water, etc."));
            sysPrint_lcdPrintPSTR(3, STR_CENTER, PSTR("(Press any key...)"));
        }

        thismenu_sm0 = -1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mFuelSensor_menu_fxKey0(void)
{
}
static inline void mFuelSensor_menu_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mFuelSensor_menu_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void mFuelSensor_menu_fxKey3(void)
{
    vscroll.stack_push();
    vscroll.base_offset_reset();

    switch (vscroll.get_markPosc())
    {
        case 0:menu_setSubMenu(MGO_MFUELSENSOR_MODELOFSENSOR_JOB);
        break;

        case 1:menu_setSubMenu(MGO_MFUELSENSOR_CALIBRATION_JOB);
        break;

        case 2:menu_setSubMenu(MGO_MFUELSENSOR_TANK_JOB);
        break;

        case 3:menu_setSubMenu(MGO_MFUELSENSOR_OUTPUTTYPE_JOB);
        break;

    }
}
static inline void mFuelSensor_menu_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setMenu(MGO_MAIN_MENU);
}

PTRFX_retVOID const mFuelSensor_menu_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_menu_fxKey0, mFuelSensor_menu_fxKey1, mFuelSensor_menu_fxKey2,
    mFuelSensor_menu_fxKey3, mFuelSensor_menu_fxKey4
};

void mFuelSensor_menu_job(void)
{
#define MENU_FUELSENSOR_OPTIONS 4

    String db_mFuelSensor_menu[MENU_FUELSENSOR_OPTIONS] =
    {
        str_clearPrint("1] MODEL OF SENSOR", 0),
        str_clearPrint("2] CALIBRATION", 0),
        str_clearPrint("3] FUEL TANK", 0),
        str_clearPrint("4] OUTPUT TYPE", 0),
    };

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_menu_fxKey);
        vscroll.init(db_mFuelSensor_menu, MENU_FUELSENSOR_OPTIONS, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        lcd.clear();
        sysPrint_lcdPrintPSTR(LCD_ROW_0, 0, PSTR("[FUEL SENSOR]"));

        thismenu_sm0 =-1;
    }

    sysPrint_lcdPrint(LCD_ROW_1, 0, fuelsensor_reading());

    vscroll.set_db(db_mFuelSensor_menu);//x local db
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}

