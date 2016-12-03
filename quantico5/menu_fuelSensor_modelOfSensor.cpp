/****************************************************************************
* Project: Battery System
*
* 2016-05-24-16.01
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

#include "menu_fuelSensor_modelOfSensor.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define FUELSENSOR_DB_MODEL_MAX 3
const struct _fuelsensor_db_modelOfsensor
{
    char name[15];//9 char +1 '\0'
}
fuelsensor_db_modelOfsensor[FUELSENSOR_DB_MODEL_MAX] PROGMEM=
{
    "JT606X   ",
    "LLS 20160",
    "LLS 20230",
};
inline static void fuelsensor_get_modelOfsensor_name(uint8_t model, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_modelOfsensor[model].name );
}
static void mFuelSensor_modelOfsensor_printNameModel(void)
{
    char buf[10];
    fuelsensor_get_modelOfsensor_name(fuelsensor.fsEE.model, buf);
    lcd.printAtPosc(1, 9, String(buf));
}
static inline void mFuelSensor_modelOfsensor_fxKey0(void)
{
}
static inline void mFuelSensor_modelOfsensor_fxKey1(void)
{
    //vscroll.sign_up();
}
static inline void mFuelSensor_modelOfsensor_fxKey2(void)
{
    //vscroll.sign_down();
}

static inline void mFuelSensor_modelOfsensor_fxKey3(void)
{
    /*
    fuelsensor.fsEE.model= vscroll.get_markPosc();
    fuelsensor_savedata();

    mFuelSensor_modelOfsensor_printNameModel();
    */
}
static inline void mFuelSensor_modelOfsensor_fxKey4(void)
{
    vscroll.stack_pop();

    menu_setSubMenu(MGO_MFUELSENSOR_MENU_JOB);
}

PTRFX_retVOID const mFuelSensor_modelOfsensor_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_modelOfsensor_fxKey0, mFuelSensor_modelOfsensor_fxKey1, mFuelSensor_modelOfsensor_fxKey2,
    mFuelSensor_modelOfsensor_fxKey3, mFuelSensor_modelOfsensor_fxKey4
};

void mFuelSensor_modelOfsensor_job(void)
{
    char buf[10];
    String db_mFuelSensor_modelOfsensor_local[FUELSENSOR_DB_MODEL_MAX];

    for (uint8_t i=0; i<FUELSENSOR_DB_MODEL_MAX; i++)
    {
        fuelsensor_get_modelOfsensor_name(i, buf);
        db_mFuelSensor_modelOfsensor_local[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 3);
    }

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_modelOfsensor_fxKey);

        lcd.clear();

        vscroll.init(db_mFuelSensor_modelOfsensor_local, FUELSENSOR_DB_MODEL_MAX, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        sysPrint_lcdPrintPSTR(0, 0, PSTR("[MODEL OF SENSOR]"));
        sysPrint_lcdPrintPSTR(1, 0, PSTR("Current: "));

        mFuelSensor_modelOfsensor_printNameModel();

        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_modelOfsensor_local);//x la naturaleza "local"
    vscroll.job();
    //lcd_show_disp(LCD_ROW_2,LCD_ROW_2+(FUELSENSOR_DB_MODEL_MAX-1));//AUTOMATIZAR CON Vertical Scroll
    lcd_show_disp(LCD_ROW_2);//AUTOMATIZAR CON Vertical Scroll
}

