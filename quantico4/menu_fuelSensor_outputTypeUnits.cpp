/****************************************************************************
* Project: Battery System
*
* 2016-05-24-15.59
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

#include "menu_fuelSensor_outputTypeUnits.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//__________________________________________________
#define MENU_FUELSENSOR_OUTPUTTYPE 2

const struct _fuelsensor_db_outputType
{
    char name[10];
}
fuelsensor_db_outputType[MENU_FUELSENSOR_OUTPUTTYPE] PROGMEM=
{
    "LENGTH",
    "VOLUME",
};

inline static void fuelsensor_get_outputType_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_outputType[type].name );
}
//__________________________________________________
//#define fuelsensor_get_outputType_name(type, buf) do{strcpy_P(buf, (char *) &fuelsensor_db_outputType[type].name );}while(0)

static void menuFuelSensor_outputType_printOuputType(void)
{
    char buf[10];
    fuelsensor_get_outputType_name(fuelsensor.fsEE.outputType.type, buf);
    lcd.printAtPosc(1, 9, String(buf));
}
static inline void menuFuelSensor_outputType_fxKey0(void)
{
}
static inline void menuFuelSensor_outputType_fxKey1(void)
{
    if (fuelsensor.fsEE.bf.enable_get_volume)
        vscroll.sign_up();
}
static inline void menuFuelSensor_outputType_fxKey2(void)
{
    if (fuelsensor.fsEE.bf.enable_get_volume)
        vscroll.sign_down();
}
static inline void menuFuelSensor_outputType_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    fuelsensor.fsEE.outputType.type = (FS_OUTPUTTYPE) posc;

    fuelsensor_savedata();

    menuFuelSensor_outputType_printOuputType();

    //
    taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;
    if (posc == 0)
    {
        taskman_fuelSensor.sm2 = 1;//to units of length
    }
    else if (posc == 1)
        taskman_fuelSensor.sm2 = 2;//to units of volume

}
static inline void menuFuelSensor_outputType_fxKey4(void)
{
    taskman_fuelSensor.sm1 = 0;
    kb_stackPop();
}

PTRFX_retVOID const menuFuelSensor_outputType_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_outputType_fxKey0, menuFuelSensor_outputType_fxKey1, menuFuelSensor_outputType_fxKey2,
    menuFuelSensor_outputType_fxKey3, menuFuelSensor_outputType_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
struct _menuFuelSensor_outputType menuFuelSensor_outputType;

void menuFuelSensor_outputType_job(void)
{
    char buf[10];

    String db_menuFuelSensor_outputType[MENU_FUELSENSOR_OUTPUTTYPE];

    for (uint8_t i=0; i<MENU_FUELSENSOR_OUTPUTTYPE; i++)
    {
        fuelsensor_get_outputType_name(i, buf);
        db_menuFuelSensor_outputType[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 5);
    }

    if (menuFuelSensor_outputType.sm0 == 0)//only at enter
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_outputType_fxKey);
        //

        menuFuelSensor_outputType.sm0++;
    }
    if (menuFuelSensor_outputType.sm0 == 1)//return from sub-menu
    {
        vscroll.init(db_menuFuelSensor_outputType, MENU_FUELSENSOR_OUTPUTTYPE, dispShowBuff, 2, FEAT_MARK_ON);
        //
        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[OUTPUT TYPE]"));
        sysPrint_lcdPrintPSTR(1, 0, PSTR("Current: "));

        menuFuelSensor_outputType_printOuputType();
        //
        menuFuelSensor_outputType.sm0 = -1;
    }

    vscroll.set_db(db_menuFuelSensor_outputType);
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_FUELSENSOR_LENGTHUNITS_NUMITEM 3
#define MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM 11
const struct _fuelsensor_db_lengthUnits
{
    char name[MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM];
}
fuelsensor_db_lengthUnits[MENU_FUELSENSOR_LENGTHUNITS_NUMITEM] PROGMEM=
{
    "Percentage",
    "Centimeter",
    "Meter     ",
};

inline static void fuelsensor_get_lengthUnits_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_lengthUnits[type].name );
}

static void menuFuelSensor_lengthUnits_printUnits(void)
{
    char buf[10];
    fuelsensor_get_lengthUnits_name(fuelsensor.fsEE.outputType.length.units, buf);
    lcd.printAtPosc(1, 9, String(buf));
}

static inline void menuFuelSensor_lengthUnits_fxKey0(void)
{
}
static inline void menuFuelSensor_lengthUnits_fxKey1(void)
{
    if ((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        vscroll.sign_up();
    }
}
static inline void menuFuelSensor_lengthUnits_fxKey2(void)
{
    if ((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        vscroll.sign_down();
    }
}
static inline void menuFuelSensor_lengthUnits_fxKey3(void)
{
    if ((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        uint8_t posc = vscroll.get_markPosc();

        fuelsensor.fsEE.outputType.length.units = posc;

        fuelsensor_savedata();

        menuFuelSensor_lengthUnits_printUnits();
    }
}
static inline void menuFuelSensor_lengthUnits_fxKey4(void)
{
    taskman_fuelSensor.sm2 = 0;//return to top level
    menuFuelSensor_outputType.sm0 = 1;//menu anterior

    kb_stackPop();
}

PTRFX_retVOID const menuFuelSensor_lengthUnits_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_lengthUnits_fxKey0, menuFuelSensor_lengthUnits_fxKey1, menuFuelSensor_lengthUnits_fxKey2,
    menuFuelSensor_lengthUnits_fxKey3, menuFuelSensor_lengthUnits_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
struct _menuFuelSensor_lengthUnits menuFuelSensor_lengthUnits;

void menuFuelSensor_lengthUnits_job(void)
{
    char buf[MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM];

    String db_menuFuelSensor_lengthUnits[MENU_FUELSENSOR_LENGTHUNITS_NUMITEM];

    for (uint8_t i=0; i<MENU_FUELSENSOR_LENGTHUNITS_NUMITEM; i++)
    {
        fuelsensor_get_lengthUnits_name(i, buf);
        db_menuFuelSensor_lengthUnits[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 2);
    }

    if (menuFuelSensor_lengthUnits.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_lengthUnits_fxKey);

        menuFuelSensor_lengthUnits.sm0++;
    }

    if (menuFuelSensor_lengthUnits.sm0 == 1)
    {
        vscroll.init(db_menuFuelSensor_lengthUnits, MENU_FUELSENSOR_LENGTHUNITS_NUMITEM, dispShowBuff, 2, FEAT_MARK_ON);

        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[UNITS OF LENGTH]"));
        sysPrint_lcdPrintPSTR(1, 0, PSTR("Current: "));

        menuFuelSensor_lengthUnits_printUnits();
        //
        menuFuelSensor_lengthUnits.sm0 = -1;
    }

    vscroll.set_db(db_menuFuelSensor_lengthUnits);
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM 2
#define MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM 8
const struct _fuelsensor_db_volumenUnits
{
    char name[MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM];
}
fuelsensor_db_volumenUnits[MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM] PROGMEM=
{
    "Gallons",
    "Liters ",
};

inline static void fuelsensor_get_volumenUnits_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_volumenUnits[type].name );
}
static void menuFuelSensor_volumenUnits_printVolume(void)
{
    char buf[10];
    fuelsensor_get_volumenUnits_name(fuelsensor.fsEE.outputType.volume.units, buf);
    lcd.printAtPosc(1, 9, String(buf));
}


static inline void menuFuelSensor_volumenUnits_fxKey0(void)
{
}
static inline void menuFuelSensor_volumenUnits_fxKey1(void)
{
    vscroll.sign_up();

}
static inline void menuFuelSensor_volumenUnits_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void menuFuelSensor_volumenUnits_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    fuelsensor.fsEE.outputType.volume.units = posc;

    fuelsensor_savedata();

    menuFuelSensor_volumenUnits_printVolume();

}
static inline void menuFuelSensor_volumenUnits_fxKey4(void)
{
    taskman_fuelSensor.sm2 = 0;//return to top level
    menuFuelSensor_outputType.sm0 = 1;//menu anterior

    kb_stackPop();
}

PTRFX_retVOID const menuFuelSensor_volumenUnits_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_volumenUnits_fxKey0, menuFuelSensor_volumenUnits_fxKey1, menuFuelSensor_volumenUnits_fxKey2,
    menuFuelSensor_volumenUnits_fxKey3, menuFuelSensor_volumenUnits_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
struct _menuFuelSensor_volumenUnits menuFuelSensor_volumenUnits;

void menuFuelSensor_volumenUnits_job(void)
{
    char buf[MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM];

    String db_menuFuelSensor_volumenUnits[MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM];

    for (uint8_t i=0; i<MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM; i++)
    {
        fuelsensor_get_volumenUnits_name(i, buf);
        db_menuFuelSensor_volumenUnits[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 2);
    }

    if (menuFuelSensor_volumenUnits.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_volumenUnits_fxKey);

        menuFuelSensor_volumenUnits.sm0++;
    }

    if (menuFuelSensor_volumenUnits.sm0 == 1)
    {
        //
        vscroll.init(db_menuFuelSensor_volumenUnits, MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM, dispShowBuff, 2, FEAT_MARK_ON);
        //
        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[UNITS OF VOLUME]"));
        sysPrint_lcdPrintPSTR(1, 0, PSTR("Current: "));

        menuFuelSensor_volumenUnits_printVolume();
        //
        menuFuelSensor_volumenUnits.sm0 = -1;
    }

    vscroll.set_db(db_menuFuelSensor_volumenUnits);
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}
