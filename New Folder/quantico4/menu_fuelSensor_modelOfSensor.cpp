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
VScroll VS_menuFuelSensor_modelOfsensor;

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
static void menuFuelSensor_modelOfsensor_printNameModel(void)
{
    char buf[10];
    fuelsensor_get_modelOfsensor_name(fuelsensor.fsEE.model, buf);
    lcd.printAtPosc(1, 9, String(buf));
}

static inline void menuFuelSensor_modelOfsensor_fxKey0(void)
{
}
static inline void menuFuelSensor_modelOfsensor_fxKey1(void)
{
    VS_menuFuelSensor_modelOfsensor.sign_up();
}
static inline void menuFuelSensor_modelOfsensor_fxKey2(void)
{
    VS_menuFuelSensor_modelOfsensor.sign_down();
}

static inline void menuFuelSensor_modelOfsensor_fxKey3(void)
{
    fuelsensor.fsEE.model= VS_menuFuelSensor_modelOfsensor.get_markPosc();
    fuelsensor_savedata();

    menuFuelSensor_modelOfsensor_printNameModel();
}
static inline void menuFuelSensor_modelOfsensor_fxKey4(void)
{
    taskman_fuelSensor.sm1 = 0;
    kb_stackPop();
}

PTRFX_retVOID const menuFuelSensor_modelOfsensor_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_modelOfsensor_fxKey0, menuFuelSensor_modelOfsensor_fxKey1, menuFuelSensor_modelOfsensor_fxKey2,
    menuFuelSensor_modelOfsensor_fxKey3, menuFuelSensor_modelOfsensor_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
struct _menuFuelSensor_modelOfsensor menuFuelSensor_modelOfsensor;

void menuFuelSensor_modelOfsensor_job(void)
{
    char buf[10];
    String db_menuFuelSensor_modelOfsensor_local[FUELSENSOR_DB_MODEL_MAX];

    for (uint8_t i=0; i<FUELSENSOR_DB_MODEL_MAX; i++)
    {
        fuelsensor_get_modelOfsensor_name(i, buf);
        db_menuFuelSensor_modelOfsensor_local[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 3);
    }

    if (menuFuelSensor_modelOfsensor.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_modelOfsensor_fxKey);

        lcd.clear();

        VS_menuFuelSensor_modelOfsensor.init(db_menuFuelSensor_modelOfsensor_local, FUELSENSOR_DB_MODEL_MAX, dispShowBuff, 2, FEAT_MARK_ON);

        sysPrint_lcdPrintPSTR(0, 0, PSTR("[MODEL OF SENSOR]"));
        sysPrint_lcdPrintPSTR(1, 0, PSTR("Current: "));

        menuFuelSensor_modelOfsensor_printNameModel();

        menuFuelSensor_modelOfsensor.sm0 = -1;
    }

    VS_menuFuelSensor_modelOfsensor.set_db(db_menuFuelSensor_modelOfsensor_local);//x la naturaleza "local"
    VS_menuFuelSensor_modelOfsensor.job();
    //lcd_show_disp(LCD_ROW_2,LCD_ROW_2+(FUELSENSOR_DB_MODEL_MAX-1));//AUTOMATIZAR CON Vertical Scroll
    lcd_show_disp(LCD_ROW_2);//AUTOMATIZAR CON Vertical Scroll
}
