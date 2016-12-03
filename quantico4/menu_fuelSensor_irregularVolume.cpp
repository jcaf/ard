/****************************************************************************
* Project: Battery System
*
* 2016-05-24-15.49
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

#include "menu_fuelSensor_irregularVolume.h"
#include "setnum.h"
#include "menu_fuelSensor_fuelTank.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _menuFuelSensor_irregular_mainMenu menuFuelSensor_irregular_mainMenu;

static inline void menuFuelSensor_irregular_mainMenu_fxKey0(void)
{
}
static inline void menuFuelSensor_irregular_mainMenu_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void menuFuelSensor_irregular_mainMenu_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void menuFuelSensor_irregular_mainMenu_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    if (posc <2) //modify disable
    {
        if (posc == 0)
            taskman_fuelSensor.sm3 = 1;
        else if (posc == 1)
            taskman_fuelSensor.sm3 = 4;

        taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;

        //
        menuFuelSensor_irregular_mainMenu.sm0 = 1; // cuando regrese

    }
}

static inline void menuFuelSensor_irregular_mainMenu_fxKey4(void)
{
    taskman_fuelSensor.sm2 = 0;
    menuFuelSensor_tank.sm0 = 1;

    kb_stackPop();
}

PTRFX_retVOID const menuFuelSensor_irregular_mainMenu_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_irregular_mainMenu_fxKey0, menuFuelSensor_irregular_mainMenu_fxKey1, menuFuelSensor_irregular_mainMenu_fxKey2,
    menuFuelSensor_irregular_mainMenu_fxKey3, menuFuelSensor_irregular_mainMenu_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
void menuFuelSensor_irregular_mainMenu_job(void)
{
#define MENU_FUELSENSOR_IRREGULAR_MAINMENU 3
    String db_menuFuelSensor_irregular_mainMenu[MENU_FUELSENSOR_IRREGULAR_MAINMENU] =
    {
        str_clearPrint("1] UNITS OF TABLE", 0),
        str_clearPrint("2] NEW TABLE", 0),
        str_clearPrint("3] MODIFY TABLE", 0),
    };

    if (menuFuelSensor_irregular_mainMenu.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_irregular_mainMenu_fxKey);



        menuFuelSensor_irregular_mainMenu.sm0++;
    }

    if (menuFuelSensor_irregular_mainMenu.sm0 == 1)
    {
        vscroll.init(db_menuFuelSensor_irregular_mainMenu, MENU_FUELSENSOR_IRREGULAR_MAINMENU, dispShowBuff, 3, FEAT_MARK_ON);

        lcd.clear();
        sysPrint_lcdPrintPSTR(LCD_ROW_0, 0, PSTR("[IRREGULAR TANK]"));

        menuFuelSensor_irregular_mainMenu.sm0 =-1;
    }

    vscroll.set_db(db_menuFuelSensor_irregular_mainMenu);//x local db
    vscroll.job();
    lcd_show_disp(LCD_ROW_1);
}

/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_FUELSENSOR_OUTPUTTYPE 2

const struct _fuelsensor_db_irregular_unitsOfTable
{
    char name[10];
}
fuelsensor_db_irregular_unitsOfTable[MENU_FUELSENSOR_OUTPUTTYPE] PROGMEM=
{
    "LENGTH",
    "VOLUME",
};

inline static void fuelsensor_get_irregular_unitsOfTable_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_irregular_unitsOfTable[type].name );
}

static inline void menuFuelSensor_irregular_unitsOfTable_fxKey0(void)
{
}
static inline void menuFuelSensor_irregular_unitsOfTable_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void menuFuelSensor_irregular_unitsOfTable_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void menuFuelSensor_irregular_unitsOfTable_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    //

    if (posc == 0)
        taskman_fuelSensor.sm3 = 2;
    else if (posc == 1)
        taskman_fuelSensor.sm3 = 3;

    taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;

    menuFuelSensor_irregular_unitsOfTable.sm0 = 1;//x cuando regrese
}
static inline void menuFuelSensor_irregular_unitsOfTable_fxKey4(void)
{
    taskman_fuelSensor.sm3 = 0;
    kb_stackPop();
}

PTRFX_retVOID const menuFuelSensor_irregular_unitsOfTable_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_irregular_unitsOfTable_fxKey0, menuFuelSensor_irregular_unitsOfTable_fxKey1, menuFuelSensor_irregular_unitsOfTable_fxKey2,
    menuFuelSensor_irregular_unitsOfTable_fxKey3, menuFuelSensor_irregular_unitsOfTable_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
struct _menuFuelSensor_irregular_unitsOfTable menuFuelSensor_irregular_unitsOfTable;

void menuFuelSensor_irregular_unitsOfTable_job(void)
{
    char buf[15];
    String db_menuFuelSensor_irregular_unitsOfTable[2];

    for (uint8_t i=0; i<2; i++)
    {
        fuelsensor_get_irregular_unitsOfTable_name(i, buf);
        db_menuFuelSensor_irregular_unitsOfTable[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 5);
    }

    if (menuFuelSensor_irregular_unitsOfTable.sm0 == 0)//only at enter
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_irregular_unitsOfTable_fxKey);
        //

        menuFuelSensor_irregular_unitsOfTable.sm0++;
    }

    if (menuFuelSensor_irregular_unitsOfTable.sm0 == 1)//return from sub-menu
    {
        vscroll.init(db_menuFuelSensor_irregular_unitsOfTable, 2, dispShowBuff, 2, FEAT_MARK_ON);
        //
        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[UNITS OF NODE]"));
        //
        menuFuelSensor_irregular_unitsOfTable.sm0 = -1;
    }

    vscroll.set_db(db_menuFuelSensor_irregular_unitsOfTable);
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_FUELSENSOR_LENGTHUNITS_NUMITEM 1
#define MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM 11
const struct _fuelsensor_db_irregular_unitsOfTable_lengthUnits
{
    char name[MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM];
}
fuelsensor_db_irregular_unitsOfTable_lengthUnits[MENU_FUELSENSOR_LENGTHUNITS_NUMITEM] PROGMEM=
{
    "Centimeter",
};

inline static void fuelsensor_get_irregular_unitsOfTable_lengthUnits_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_irregular_unitsOfTable_lengthUnits[type].name );
}

static void menuFuelSensor_irregular_unitsOfTable_lengthUnits_printUnits(void)
{
    char buf[10];
    fuelsensor_get_irregular_unitsOfTable_lengthUnits_name(fuelsensor.fsEE.tank.irregular.spline.node_units.length.units, buf);
    lcd.printAtPosc(1, 9, String(buf));
}

static inline void menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey0(void)
{
}
static inline void menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey1(void)
{
    //vscroll.sign_up();
}
static inline void menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey2(void)
{
    //vscroll.sign_down();
}
static inline void menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey3(void)
{
}
static inline void menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey4(void)
{
    taskman_fuelSensor.sm3 = 1;
    kb_stackPop();
}

PTRFX_retVOID const menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey0, menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey1, menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey2,
    menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey3, menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
struct _menuFuelSensor_irregular_unitsOfTable_lengthUnits menuFuelSensor_irregular_unitsOfTable_lengthUnits;

void menuFuelSensor_irregular_unitsOfTable_lengthUnits_job(void)
{
    char buf[MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM];

    String db_menuFuelSensor_irregular_unitsOfTable_lengthUnits[MENU_FUELSENSOR_LENGTHUNITS_NUMITEM];

    for (uint8_t i=0; i<MENU_FUELSENSOR_LENGTHUNITS_NUMITEM; i++)
    {
        fuelsensor_get_irregular_unitsOfTable_lengthUnits_name(i, buf);
        db_menuFuelSensor_irregular_unitsOfTable_lengthUnits[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 2);
    }

    if (menuFuelSensor_irregular_unitsOfTable_lengthUnits.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey);

        menuFuelSensor_irregular_unitsOfTable_lengthUnits.sm0++;
    }

    if (menuFuelSensor_irregular_unitsOfTable_lengthUnits.sm0 == 1)
    {
        vscroll.init(db_menuFuelSensor_irregular_unitsOfTable_lengthUnits, MENU_FUELSENSOR_LENGTHUNITS_NUMITEM, dispShowBuff, 1, FEAT_MARK_ON);

        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[UNITS OF LENGTH]"));
        sysPrint_lcdPrintPSTR(1, 0, PSTR("Current: "));

        menuFuelSensor_irregular_unitsOfTable_lengthUnits_printUnits();
        //
        menuFuelSensor_irregular_unitsOfTable_lengthUnits.sm0 = -1;
    }

    vscroll.set_db(db_menuFuelSensor_irregular_unitsOfTable_lengthUnits);
    vscroll.job();
    lcd_show_disp(LCD_ROW_2, LCD_ROW_2);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM 2
#define MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM 8
const struct _fuelsensor_db_irregular_unitsOfTable_volumenUnits
{
    char name[MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM];
}
fuelsensor_db_irregular_unitsOfTable_volumenUnits[MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM] PROGMEM=
{
    "Gallons",
    "Liters",
};

inline static void fuelsensor_get_irregular_unitsOfTable_volumenUnits_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_irregular_unitsOfTable_volumenUnits[type].name );
}
static void menuFuelSensor_irregular_unitsOfTable_volumenUnits_printVolume(void)
{
    char buf[10];
    fuelsensor_get_irregular_unitsOfTable_volumenUnits_name(fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units, buf);
    lcd.printAtPosc(1, 9, String(buf)+" ");
}

static inline void menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey0(void)
{
}
static inline void menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey1(void)
{
    vscroll.sign_up();

}
static inline void menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units = posc;

    fuelsensor_savedata();

    menuFuelSensor_irregular_unitsOfTable_volumenUnits_printVolume();

}
static inline void menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey4(void)
{
    taskman_fuelSensor.sm3 = 1;
    kb_stackPop();
}

PTRFX_retVOID const menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey0, menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey1, menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey2,
    menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey3, menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
struct _menuFuelSensor_irregular_unitsOfTable_volumenUnits menuFuelSensor_irregular_unitsOfTable_volumenUnits;

void menuFuelSensor_irregular_unitsOfTable_volumenUnits_job(void)
{
    char buf[MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM];

    String db_menuFuelSensor_irregular_unitsOfTable_volumenUnits[MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM];

    for (uint8_t i=0; i<MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM; i++)
    {
        fuelsensor_get_irregular_unitsOfTable_volumenUnits_name(i, buf);
        db_menuFuelSensor_irregular_unitsOfTable_volumenUnits[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 2);
    }

    if (menuFuelSensor_irregular_unitsOfTable_volumenUnits.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey);

        menuFuelSensor_irregular_unitsOfTable_volumenUnits.sm0++;
    }

    if (menuFuelSensor_irregular_unitsOfTable_volumenUnits.sm0 == 1)
    {
        //
        vscroll.init(db_menuFuelSensor_irregular_unitsOfTable_volumenUnits, MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM, dispShowBuff, 2, FEAT_MARK_ON);
        //
        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[UNITS OF VOLUME]"));
        sysPrint_lcdPrintPSTR(1, 0, PSTR("Current: "));

        menuFuelSensor_irregular_unitsOfTable_volumenUnits_printVolume();
        //
        menuFuelSensor_irregular_unitsOfTable_volumenUnits.sm0 = -1;
    }

    vscroll.set_db(db_menuFuelSensor_irregular_unitsOfTable_volumenUnits);
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _menuFuelSensor_irregularVolume menuFuelSensor_irregularVolume;


static void setnum_digit_blink_print(uint8_t blink_toggle, uint8_t setnum_digit_position);
static void setnum_check_lower_boundary(void);
static void print_X(float X);
static void print_A(float A);

//#define PARAM_STR_LENGTH (7+1)
//char PARAM0str[PARAM_STR_LENGTH];//="0000.00";
//char PARAM1str[PARAM_STR_LENGTH];//="0000.00";
//char PARAM2str[PARAM_STR_LENGTH];//="0000.00";


//#define DTOSTRF_SIGN_POS 1

#define X_STR_LENGTH (6+1)//6+\0
#define A_STR_LENGTH (7+1)//7+\0

#define X_STR_DP_PRECISION 2
#define A_STR_DP_PRECISION 2

//original
//char Xstr[X_STR_LENGTH]="000.00" ;
//char Astr[A_STR_LENGTH]="0000.00";

//reutilizando el codigo
#define Xstr PARAM0str
#define Astr PARAM1str

//char Xstr[PARAM_STR_LENGTH]="000.00" ;
//char Astr[PARAM_STR_LENGTH]="0000.00";

#define X_STR_DIG_POS_DP 3
#define A_STR_DIG_POS_DP 4

#define X_STR_DIG_POS_MAX (X_STR_LENGTH-2)
#define A_STR_DIG_POS_MAX (A_STR_LENGTH-2)

#define SETNUM_PARAM_HEIGHT 0
#define SETNUM_PARAM_VOLUME 1

static inline void menuFuelSensor_irregularVolume_fxKey0(void)
{
}
static inline void menuFuelSensor_irregularVolume_fxKey1(void)
{

    char *XAstr;

    if (setnum.param == SETNUM_PARAM_HEIGHT)
        XAstr = Xstr;
    else
        XAstr = Astr;

    if (--XAstr[setnum.digit.position]< '0')
        XAstr[setnum.digit.position] = '9';

    setnum_digit_blink_set2noblank();
}

static inline void menuFuelSensor_irregularVolume_fxKey2(void)
{
    char *XAstr;

    if (setnum.param == SETNUM_PARAM_HEIGHT)
        XAstr = Xstr;
    else
        XAstr = Astr;

    if (++XAstr[setnum.digit.position] > '9')
        XAstr[setnum.digit.position] = '0';

    setnum_digit_blink_set2noblank();
}

static inline void menuFuelSensor_irregularVolume_fxKey3(void)
{
    setnum.digit.position++;

    setnum_check_lower_boundary();

    if (setnum.param == SETNUM_PARAM_HEIGHT)
    {
        if (setnum.digit.position == X_STR_DIG_POS_DP)//skip decimal point
            setnum.digit.position++;

        if (setnum.digit.position>X_STR_DIG_POS_MAX)
        {
            setnum_digit_blink_print(BLINK_TOGGLE_NOBLANK, X_STR_DIG_POS_MAX);//fix latest digit before to leave

            setnum.digit.position = 0;
            setnum.param++;
        }
    }
    if (setnum.param == SETNUM_PARAM_VOLUME)
    {
        if (setnum.digit.position == A_STR_DIG_POS_DP)//skip decimal point
            setnum.digit.position++;

        if (setnum.digit.position>A_STR_DIG_POS_MAX)
        {
            setnum_digit_blink_print(BLINK_TOGGLE_NOBLANK, A_STR_DIG_POS_MAX);//fix latest digit before to leave

            setnum.digit.position = 0;
            setnum.param--;
        }
    }

    setnum_digit_blink_set2blank();
}

static inline void menuFuelSensor_irregularVolume_fxKey4(void)
{


    uint8_t node_counter = fuelsensor.fsEE.tank.irregular.spline.node_counter;

    //1.- check consistency
    taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;

    taskman_fuelSensor.sm3 = 5; //set default -> Error

    setnum.bf.error_equalzero_length = 0;
    setnum.bf.error_equalzero_volume = 0;
    setnum.bf.error_consistency_lenght = 0;
    setnum.bf.error_consistency_volume = 0;

    setnum_check_lower_boundary();//update Xstr and Astr!

    float Xentered = strtod(Xstr, (char**)0);
    float Aentered = strtod(Astr, (char**)0);

    if (Xentered == 0.0f)
        setnum.bf.error_equalzero_length = 1;

    if (Aentered == 0.0f)
        setnum.bf.error_equalzero_volume = 1;

    if (node_counter > 0)
    {
        if (Xentered == fuelsensor.fsEE.tank.irregular.spline.node[node_counter-1].X)
            setnum.bf.error_consistency_lenght = 1;

        if (Aentered == fuelsensor.fsEE.tank.irregular.spline.node[node_counter-1].A)
            setnum.bf.error_consistency_volume = 1;
    }
    //------------------------------------------------------------------------------
    if ( (setnum.bitflag & 0x1E) == 0)//NO ERRORS
    {
        if (fuelsensor.fsEE.tank.irregular.spline.node_counter > SPLINE_NODES_MAX)
        {
            /*
            print message("TABLE HAS REACH THE MAX NUM"))

            */
        }
        else
        {
            fuelsensor.fsEE.tank.irregular.spline.node[node_counter].X = Xentered;
            fuelsensor.fsEE.tank.irregular.spline.node[node_counter].A = Aentered;

            //save & continue entering new node?
            //taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;
            taskman_fuelSensor.sm3 = 6;
            menuFuelSensor_irregularVolume.sm1 = 0;
        }
    }

}

PTRFX_retVOID const menuFuelSensor_irregularVolume_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_irregularVolume_fxKey0, menuFuelSensor_irregularVolume_fxKey1, menuFuelSensor_irregularVolume_fxKey2,
    menuFuelSensor_irregularVolume_fxKey3, menuFuelSensor_irregularVolume_fxKey4
};

static void menuFuelSensor_irregularVolume_fill_spline_nodes(void);

void menuFuelSensor_irregularVolume_job(void)
{
    if (menuFuelSensor_irregularVolume.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_irregularVolume_fxKey);

        menuFuelSensor_irregularVolume.sm1 = 0x0;
        menuFuelSensor_irregularVolume.sm0 = -1;

//        Xstr[PARAM_STR_LENGTH]="000.00" ;
//        Astr[PARAM_STR_LENGTH]="0000.00";

        fuelsensor.fsEE.tank.irregular.spline.node_counter = 0;
    }

    if (menuFuelSensor_irregularVolume.sm1 == 0)
    {
        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[NODE-TABLE]"));

        //sysPrint_lcdPrintPSTR(2, 3, PSTR("[cm]  , [gallons]"));
        sysPrint_lcdPrintPSTR(2, 3, PSTR("[cm]  , "));

        //
        char buf[10];
        fuelsensor_get_irregular_unitsOfTable_volumenUnits_name(fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units, buf);
        lcd.printAtPosc(2, 11, "["+String(buf)+"]");
        //
        lcd.printAtPosc(3, 9, ",");



        //carga lo que haya grabado
//        for (uint8_t i=0; i< SPLINE_NODES_MAX; i++)
//        {
//            fuelsensor.fsEE.tank.irregular.spline.node[i].X = 0.0f;
//            fuelsensor.fsEE.tank.irregular.spline.node[i].A = 0.0f;
//        }
        //fuelsensor_savedata();

        menuFuelSensor_irregularVolume.sm1++;
    }
    if (menuFuelSensor_irregularVolume.sm1 == 1)
    {
        //setnum_digit_blink_set2blank();//begin with blink "blank space"
        setnum.param = SETNUM_PARAM_HEIGHT;
        setnum.digit.position = 0;

        uint8_t node_counter = fuelsensor.fsEE.tank.irregular.spline.node_counter;
        sysPrint_lcdPrint(1, STR_CENTER, "NODE "+String( node_counter+1));

        if (node_counter > 0)
        {
            fuelsensor.fsEE.tank.irregular.spline.node[node_counter].X = fuelsensor.fsEE.tank.irregular.spline.node[node_counter-1].X;
            fuelsensor.fsEE.tank.irregular.spline.node[node_counter].A = fuelsensor.fsEE.tank.irregular.spline.node[node_counter-1].A;
        }
        print_X(fuelsensor.fsEE.tank.irregular.spline.node[node_counter].X);
        print_A(fuelsensor.fsEE.tank.irregular.spline.node[node_counter].A);

        menuFuelSensor_irregularVolume.sm1 = -1;
    }
    //-------- blinking ----------
    if (setnum.digit.blink.sm0 == 0)
    {
        if ( (millis() - setnum.digit.blink.last_millis) > 500)
        {
            setnum.digit.blink.bf.toggle = !setnum.digit.blink.bf.toggle;
            setnum.digit.blink.sm0++;
        }
    }
    if (setnum.digit.blink.sm0 == 1)
    {
        setnum_digit_blink_print(setnum.digit.blink.bf.toggle, setnum.digit.position);
        //
        setnum.digit.blink.last_millis = millis();
        setnum.digit.blink.sm0 = 0;
    }
}

static void setnum_check_lower_boundary(void)
{
    char *p;
    float n_entered, limit;
    uint8_t node_counter = fuelsensor.fsEE.tank.irregular.spline.node_counter;

    if (setnum.param == SETNUM_PARAM_HEIGHT)
    {
        p = Xstr;
        limit = fuelsensor.fsEE.tank.irregular.spline.node[node_counter].X;
    }
    else
    {
        p = Astr;
        limit = fuelsensor.fsEE.tank.irregular.spline.node[node_counter].A;
    }

    n_entered = strtod(p, (char**)0);

    if (n_entered < limit)
        n_entered = limit;

    if (setnum.param == SETNUM_PARAM_HEIGHT)
        print_X(n_entered);
    else
        print_A(n_entered);
}

static void print_X(float X)
{
    dtostrf(X, X_STR_LENGTH-DTOSTRF_SIGN_POS, X_STR_DP_PRECISION, Xstr);
    dtostrf_fill_zero(Xstr);
    lcd.printAtPosc(3,2, String(Xstr));
}
static void print_A(float A)
{
    dtostrf(A, A_STR_LENGTH-DTOSTRF_SIGN_POS, A_STR_DP_PRECISION, Astr);
    dtostrf_fill_zero(Astr);
    lcd.printAtPosc(3,12, String(Astr));
}


static void setnum_digit_blink_print(uint8_t blink_toggle, uint8_t setnum_digit_position )
{
    uint8_t col;
    char xa_digit_str=' ';
    char *XAstr;

    if (setnum.param == SETNUM_PARAM_HEIGHT)
    {
        col=2;
        XAstr = Xstr;
    }
    else
    {
        col=12;
        XAstr = Astr;
    }

    if (blink_toggle == BLINK_TOGGLE_NOBLANK)
        xa_digit_str = XAstr[setnum_digit_position];

    col += setnum_digit_position;
    lcd.printAtPosc(3, col,String(xa_digit_str));
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//VScroll vscroll;

struct _menuFuelSensor_irregularVolume_errorConsistency menuFuelSensor_irregularVolume_errorConsistency;

static void menuFuelSensor_irregularVolume_errorConsistency_fxKey0(void)
{
    if (menuFuelSensor_irregularVolume_errorConsistency.opt == 0)
    {
        menuFuelSensor_irregularVolume_errorConsistency.opt++;//skip the error
        menuFuelSensor_irregularVolume_errorConsistency.sm1 = 0;
    }
}
static void menuFuelSensor_irregularVolume_errorConsistency_fxKey1(void)
{
    if (menuFuelSensor_irregularVolume_errorConsistency.opt == 0)
    {
        menuFuelSensor_irregularVolume_errorConsistency.opt++;//skip the error
        menuFuelSensor_irregularVolume_errorConsistency.sm1 = 0;
    }
    else
    {
        vscroll.sign_up();
    }
}
static void menuFuelSensor_irregularVolume_errorConsistency_fxKey2(void)
{
    if (menuFuelSensor_irregularVolume_errorConsistency.opt == 0)
    {
        menuFuelSensor_irregularVolume_errorConsistency.opt++;//skip the error
        menuFuelSensor_irregularVolume_errorConsistency.sm1 = 0;
    }
    else
    {
        vscroll.sign_down();
    }
}
static void menuFuelSensor_irregularVolume_errorConsistency_fxKey3(void)
{
    if (menuFuelSensor_irregularVolume_errorConsistency.opt == 0)
    {
        menuFuelSensor_irregularVolume_errorConsistency.opt++;//skip the error
        menuFuelSensor_irregularVolume_errorConsistency.sm1 = 0;
    }
    else
    {
        uint8_t posc = vscroll.get_markPosc();

        if (posc == 0)
        {
            menuFuelSensor_irregularVolume.sm1 = 0;
            taskman_fuelSensor.sm3 = 4;//0;
            kb_stackPop();
        }
        else
        {
            //check minimum
            //check min
            if (fuelsensor.fsEE.tank.irregular.spline.node_counter < SPLINE_NODES_MIN)
            {
                taskman_fuelSensor.sm3 = 7;//3;//show minNode
                taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;
            }
            else
            {
                //exit to menu "IRREGULAR TANK"
                kb_stackPop();
                kb_stackPop();
                //taskman_fuelSensor.sm2 = 0;
                taskman_fuelSensor.sm3 = 0;
                //
            }
        }
    }
}
static void menuFuelSensor_irregularVolume_errorConsistency_fxKey4(void)
{
    if (menuFuelSensor_irregularVolume_errorConsistency.opt == 0)
    {
        menuFuelSensor_irregularVolume_errorConsistency.opt++;//skip the error
        menuFuelSensor_irregularVolume_errorConsistency.sm1 = 0;
    }
    else
    {

    }
}

PTRFX_retVOID const menuFuelSensor_irregularVolume_errorConsistency_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_irregularVolume_errorConsistency_fxKey0, menuFuelSensor_irregularVolume_errorConsistency_fxKey1, menuFuelSensor_irregularVolume_errorConsistency_fxKey2,
    menuFuelSensor_irregularVolume_errorConsistency_fxKey3, menuFuelSensor_irregularVolume_errorConsistency_fxKey4
};

void menuFuelSensor_irregularVolume_errorConsistency_job(void)
{
    uint8_t node_counter = fuelsensor.fsEE.tank.irregular.spline.node_counter;

    if (menuFuelSensor_irregularVolume_errorConsistency.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_irregularVolume_errorConsistency_fxKey);

        lcd.clear();

        sysPrint_lcdPrintPSTR(LCD_ROW_0, 0, PSTR("[ERROR]"));
        //
        menuFuelSensor_irregularVolume_errorConsistency.opt = 0x00;
        menuFuelSensor_irregularVolume_errorConsistency.sm1 = 0;
        //
        menuFuelSensor_irregularVolume_errorConsistency.sm0 = -1;
    }

    if (menuFuelSensor_irregularVolume_errorConsistency.opt == 0)
    {
        if (menuFuelSensor_irregularVolume_errorConsistency.sm1 == 0)
        {
            String msg="";

            if ( (setnum.bf.error_equalzero_length) || (setnum.bf.error_consistency_lenght) )
                msg+="[Length] ";

            if ( (setnum.bf.error_equalzero_volume) || (setnum.bf.error_consistency_volume) )
                msg+="[Volume]";

            sysPrint_lcdPrint(LCD_ROW_1, STR_CENTER, msg);

            if ( (setnum.bitflag &  (0x03<<3)) != 0)
            {
                sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("must be different"));
                sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("to zero"));
            }
            else if ( (setnum.bitflag &  (0x03<<1) )!= 0)
            {
                sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("must be greater"));
                sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("than previous node"));
            }
            menuFuelSensor_irregularVolume_errorConsistency.sm1 = -1;
        }
    }
    else
    {
        String db_vscroll[2] =
        {
            str_clearPrint("1] Yes", 7),
            str_clearPrint("2] No", 7),
        };

        if (menuFuelSensor_irregularVolume_errorConsistency.sm1 == 0)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("Discard & continue"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("entering new node?"));

            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

            menuFuelSensor_irregularVolume_errorConsistency.sm1 = -1;
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        lcd_show_disp(LCD_ROW_2);
    }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

struct _menuFuelSensor_irregularVolume_save_continue menuFuelSensor_irregularVolume_save_continue;

static void menuFuelSensor_irregularVolume_save_continue_fxKey0(void)
{
}
static void menuFuelSensor_irregularVolume_save_continue_fxKey1(void)
{
    vscroll.sign_up();
}
static void menuFuelSensor_irregularVolume_save_continue_fxKey2(void)
{
    vscroll.sign_down();
}
static void menuFuelSensor_irregularVolume_save_continue_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    if (menuFuelSensor_irregularVolume_save_continue.opt == 0)//step 0
    {
        if (posc == 0)//Yes
        {
            fuelsensor.fsEE.tank.irregular.spline.node_counter++;

            if (fuelsensor.fsEE.tank.irregular.spline.node_counter == (SPLINE_NODES_MAX))
            {
                taskman_fuelSensor.sm3 = 8;//visualizar maximum...
                taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;
            }
            else
            {
                menuFuelSensor_irregularVolume_save_continue.sm1 = 0;
                menuFuelSensor_irregularVolume_save_continue.opt++;//next step
            }
        }
        else//No
        {
            if (fuelsensor.fsEE.tank.irregular.spline.node_counter < SPLINE_NODES_MIN)
            {
                taskman_fuelSensor.sm3 = 7;//show minNode
                taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;
            }
            else
            {
                menuFuelSensor_irregularVolume_save_continue.sm1 = 0;
                menuFuelSensor_irregularVolume_save_continue.opt++;
            }
        }
    }
    else//step 1
    {
        if (posc == 0)
        {
            taskman_fuelSensor.sm3 = 4;
            menuFuelSensor_irregularVolume.sm1 = 0;
            kb_stackPop();
        }
        else
        {
            //check min
            if (fuelsensor.fsEE.tank.irregular.spline.node_counter < SPLINE_NODES_MIN)
            {
                taskman_fuelSensor.sm3 = 7;//show minNode
                taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;
            }
            else
            {
                //exit to menu "IRREGULAR TANK"
                kb_stackPop();
                kb_stackPop();
                taskman_fuelSensor.sm3 = 0;
            }
        }
    }

    //-------------------------------------
    if (fuelsensor.fsEE.tank.irregular.spline.node_counter < SPLINE_NODES_MIN)
    {
        fuelsensor.fsEE.bf.irregular_spline_nodetable_consistent = 0;
    }
    else
    {
        fuelsensor.fsEE.bf.irregular_spline_nodetable_consistent = 1;
        spline_poly_get_coeffs();

    }
    fuelsensor_check_consistency();

    fuelsensor_savedata();//tambien save nodes 1,2 entered + counter

    //-------------------------------------

}
static void menuFuelSensor_irregularVolume_save_continue_fxKey4(void)
{
}

PTRFX_retVOID const menuFuelSensor_irregularVolume_save_continue_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_irregularVolume_save_continue_fxKey0, menuFuelSensor_irregularVolume_save_continue_fxKey1, menuFuelSensor_irregularVolume_save_continue_fxKey2,
    menuFuelSensor_irregularVolume_save_continue_fxKey3, menuFuelSensor_irregularVolume_save_continue_fxKey4
};

void menuFuelSensor_irregularVolume_save_continue_job(void)
{
    if (menuFuelSensor_irregularVolume_save_continue.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_irregularVolume_save_continue_fxKey);

        menuFuelSensor_irregularVolume_save_continue.opt = 0;
        menuFuelSensor_irregularVolume_save_continue.sm1 = 0;

        menuFuelSensor_irregularVolume_save_continue.sm0 = -1;
    }

    String db_vscroll[2] =
    {
        str_clearPrint("1] Yes", 7),
        str_clearPrint("2] No", 7),
    };

    if (menuFuelSensor_irregularVolume_save_continue.opt == 0)
    {
        if (menuFuelSensor_irregularVolume_save_continue.sm1 == 0)
        {
            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("Save node?"));

            menuFuelSensor_irregularVolume_save_continue.sm1 = -1;
        }
    }
    else
    {
        if (menuFuelSensor_irregularVolume_save_continue.sm1 == 0)
        {
            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

            lcd.clear();

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("Continue entering"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("new node?"));//then EXIT

            menuFuelSensor_irregularVolume_save_continue.sm1 = -1;
        }
    }
    vscroll.set_db(db_vscroll);
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
struct _menuFuelSensor_irregularVolume_minNode menuFuelSensor_irregularVolume_minNode;

static void menuFuelSensor_irregularVolume_minNode_fxKey0(void)
{
    if (menuFuelSensor_irregularVolume_minNode.opt == 0)
    {
        menuFuelSensor_irregularVolume_minNode.opt++;//skip the warning
        menuFuelSensor_irregularVolume_minNode.sm1 = 0;
    }
}
static void menuFuelSensor_irregularVolume_minNode_fxKey1(void)
{
    if (menuFuelSensor_irregularVolume_minNode.opt == 0)
    {
        menuFuelSensor_irregularVolume_minNode.opt++;//skip the warning
        menuFuelSensor_irregularVolume_minNode.sm1 = 0;
    }
    else
    {
        vscroll.sign_up();
    }
}
static void menuFuelSensor_irregularVolume_minNode_fxKey2(void)
{
    if (menuFuelSensor_irregularVolume_minNode.opt == 0)
    {
        menuFuelSensor_irregularVolume_minNode.opt++;//skip the warning
        menuFuelSensor_irregularVolume_minNode.sm1 = 0;
    }
    else
    {
        vscroll.sign_down();
    }
}
static void menuFuelSensor_irregularVolume_minNode_fxKey3(void)
{
    if (menuFuelSensor_irregularVolume_minNode.opt == 0)
    {
        menuFuelSensor_irregularVolume_minNode.opt++;//skip the warning
        menuFuelSensor_irregularVolume_minNode.sm1 = 0;
    }
    else
    {
        uint8_t posc = vscroll.get_markPosc();

        if (posc == 0)
        {
            //exit to menu "IRREGULAR TANK"
            kb_stackPop();
            kb_stackPop();
            kb_stackPop();
            //taskman_fuelSensor.sm2 = 0;
            taskman_fuelSensor.sm3 = 0;
            //
        }
        else
        {
            menuFuelSensor_irregularVolume.sm1 = 0;
            taskman_fuelSensor.sm3 = 4;// 0;
            kb_stackPop();
            kb_stackPop();
        }
    }
}
static void menuFuelSensor_irregularVolume_minNode_fxKey4(void)
{
    if (menuFuelSensor_irregularVolume_minNode.opt == 0)
    {
        menuFuelSensor_irregularVolume_minNode.opt++;//skip the warning
        menuFuelSensor_irregularVolume_minNode.sm1 = 0;
    }
    else
    {

    }
}

PTRFX_retVOID const menuFuelSensor_irregularVolume_minNode_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_irregularVolume_minNode_fxKey0, menuFuelSensor_irregularVolume_minNode_fxKey1, menuFuelSensor_irregularVolume_minNode_fxKey2,
    menuFuelSensor_irregularVolume_minNode_fxKey3, menuFuelSensor_irregularVolume_minNode_fxKey4
};

void menuFuelSensor_irregularVolume_minNode_job(void)
{
    uint8_t node_counter = fuelsensor.fsEE.tank.irregular.spline.node_counter;

    if (menuFuelSensor_irregularVolume_minNode.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_irregularVolume_minNode_fxKey);

        //
        menuFuelSensor_irregularVolume_minNode.opt = 0x00;
        menuFuelSensor_irregularVolume_minNode.sm1 = 0;
        //
        menuFuelSensor_irregularVolume_minNode.sm0 = -1;
    }

    if (menuFuelSensor_irregularVolume_minNode.opt == 0)
    {
        if (menuFuelSensor_irregularVolume_minNode.sm1 == 0)
        {
            lcd.clear();

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[WARNING]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Number of node is"));
            sysPrint_lcdPrint(LCD_ROW_2, STR_CENTER, "lower than "+ String(SPLINE_NODES_MIN));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("(Press any key...)"));

            menuFuelSensor_irregularVolume_minNode.sm1 = -1;
        }
    }
    else
    {
        String db_vscroll[2] =
        {
            str_clearPrint("1] Yes", 7),
            str_clearPrint("2] No", 7),
        };

        if (menuFuelSensor_irregularVolume_minNode.sm1 == 0)
        {
            lcd.clear();

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("Exit & discard the"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("node-table anyway?"));

            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

            menuFuelSensor_irregularVolume_minNode.sm1 = -1;
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        lcd_show_disp(LCD_ROW_2);
    }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
struct _menuFuelSensor_irregularVolume_maximum_size menuFuelSensor_irregularVolume_maximum_size;

void _escape(void)
{
    //exit to menu "IRREGULAR TANK"
    kb_stackPop();
    kb_stackPop();
    kb_stackPop();

    taskman_fuelSensor.sm3 = 0;
    //
}
static void menuFuelSensor_irregularVolume_maximum_size_fxKey0(void)
{
    _escape();
}
static void menuFuelSensor_irregularVolume_maximum_size_fxKey1(void)
{
    _escape();
}
static void menuFuelSensor_irregularVolume_maximum_size_fxKey2(void)
{
    _escape();
}
static void menuFuelSensor_irregularVolume_maximum_size_fxKey3(void)
{
    _escape();
}
static void menuFuelSensor_irregularVolume_maximum_size_fxKey4(void)
{
    _escape();
}

PTRFX_retVOID const menuFuelSensor_irregularVolume_maximum_size_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_irregularVolume_maximum_size_fxKey0, menuFuelSensor_irregularVolume_maximum_size_fxKey1, menuFuelSensor_irregularVolume_maximum_size_fxKey2,
    menuFuelSensor_irregularVolume_maximum_size_fxKey3, menuFuelSensor_irregularVolume_maximum_size_fxKey4
};

void menuFuelSensor_irregularVolume_maximum_size_job(void)
{
    if (menuFuelSensor_irregularVolume_maximum_size.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_irregularVolume_maximum_size_fxKey);

        lcd.clear();

        sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("Node-table has"));
        sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("reached maximum"));
        sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("size. Press any"));
        sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("key to exit..."));

        menuFuelSensor_irregularVolume_maximum_size.sm0 = -1;
    }
}
