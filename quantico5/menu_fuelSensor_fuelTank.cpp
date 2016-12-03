/****************************************************************************
* Project: Battery System
*
* 2016-05-24-16.02
* jcaf @ jcafOpenSuse
*
* Copyright 2013 Juan Carlos Agüero Flores
*
* Licensed under the Apache License, Version 2.0 (the "License");
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mFuelSensor_tank_fxKey0(void)
{
}
static inline void mFuelSensor_tank_fxKey1(void)
{
    if ((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        vscroll.sign_up();
    }
}
static inline void mFuelSensor_tank_fxKey2(void)
{
    if ((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        vscroll.sign_down();
    }
}
static inline void mFuelSensor_tank_fxKey3(void)
{
    if (1)//((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        vscroll.stack_push();
        vscroll.base_offset_reset();

        switch (vscroll.get_markPosc())
        {
            case 0: menu_setSubMenu(MGO_MFUELSENSOR_TYPEOFTANK_JOB);break;
            case 1:
                    if (fuelsensor.fsEE.tank.type == 0)    //RECTANGULAR
                        menu_setSubMenu(MGO_MFUELSENSOR_RECTANGULARVOLUME_JOB);//taskman_fuelSensor.sm2 = 2;//Rectangular Volume Calc
                    else
                        menu_setSubMenu(MGO_MFUELSENSOR_IRREGULAR_MENU_JOB);//taskman_fuelSensor.sm2 = 3;//Irregular Volume Calc - SPLINE
                break;
        }
    }
}
static inline void mFuelSensor_tank_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setSubMenu(MGO_MFUELSENSOR_MENU_JOB);
}

PTRFX_retVOID const mFuelSensor_tank_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_tank_fxKey0, mFuelSensor_tank_fxKey1, mFuelSensor_tank_fxKey2,
    mFuelSensor_tank_fxKey3, mFuelSensor_tank_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
void mFuelSensor_tank_job(void)
{
    #define MENU_FUELSENSOR_TANK 2
    String db_mFuelSensor_tank_local[MENU_FUELSENSOR_TANK]=
    {
        str_clearPrint("[1] TYPE OF TANK", 1),
        str_clearPrint("[2] VOLUME CALC.", 1),
    };
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_tank_fxKey);
        vscroll.init(db_mFuelSensor_tank_local, MENU_FUELSENSOR_TANK, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);
        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[FUEL TANK]"));

        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_tank_local);//x la naturaleza "local"
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MENU_FUELSENSOR_TYPEOFTANK 2
//__________________________________________________
const struct _fuelsensor_db_typeOfTank
{
    char name[15];
}
fuelsensor_db_typeOfTank[MENU_FUELSENSOR_TYPEOFTANK] PROGMEM=
{
    "RECTANGULAR",
    "IRREGULAR  ",
};
inline static void fuelsensor_get_typeOfTank_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_typeOfTank[type].name );
}
//__________________________________________________
static void mFuelSensor_typeOfTank_printCurrent(void)
{
    char buf[15];
    fuelsensor_get_typeOfTank_name(fuelsensor.fsEE.tank.type, buf);
    lcd.printAtPosc(1, 9, String(buf));
}

static inline void mFuelSensor_typeOfTank_fxKey0(void)
{
}
static inline void mFuelSensor_typeOfTank_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mFuelSensor_typeOfTank_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mFuelSensor_typeOfTank_fxKey3(void)
{
    fuelsensor.fsEE.tank.type = vscroll.get_markPosc();//0=rectangular 1=irregular
    fuelsensor_check_consistency();
    fuelsensor_savedata();

    mFuelSensor_typeOfTank_printCurrent();
}
static inline void mFuelSensor_typeOfTank_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_TANK_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_typeOfTank_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_typeOfTank_fxKey0, mFuelSensor_typeOfTank_fxKey1, mFuelSensor_typeOfTank_fxKey2,
    mFuelSensor_typeOfTank_fxKey3, mFuelSensor_typeOfTank_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
void mFuelSensor_typeOfTank_job(void)
{
    char buf[15];
    String db_mFuelSensor_typeOfTank_local[MENU_FUELSENSOR_TYPEOFTANK];

    for (uint8_t i=0; i<MENU_FUELSENSOR_TYPEOFTANK; i++)
    {
        fuelsensor_get_typeOfTank_name(i, buf);
        db_mFuelSensor_typeOfTank_local[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 3);
    }

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_typeOfTank_fxKey);
        vscroll.init(db_mFuelSensor_typeOfTank_local, MENU_FUELSENSOR_TYPEOFTANK, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);
        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[TYPE OF TANK]"));
        sysPrint_lcdPrintPSTR(1, 0, PSTR("Current: "));

        mFuelSensor_typeOfTank_printCurrent();

        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_typeOfTank_local);//x la naturaleza "local"
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}

