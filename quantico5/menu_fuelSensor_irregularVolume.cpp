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
static inline void mFuelSensor_irregular_menu_fxKey0(void)
{
}
static inline void mFuelSensor_irregular_menu_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mFuelSensor_irregular_menu_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void mFuelSensor_irregular_menu_fxKey3(void)
{
//    vscroll.stack_push();
//    vscroll.base_offset_reset();

    switch (vscroll.get_markPosc())
    {
        case 0:
            vscroll.stack_push();
            vscroll.base_offset_reset();

            menu_setSubMenu(MGO_MFUELSENSOR_IRREGULAR_UNITSOFTABLE_JOB);
            break;

        case 1:
            vscroll.stack_push();
            vscroll.base_offset_reset();

            menu_setSubMenu(MGO_MFUELSENSOR_IRREGULARVOL_NEWTABLE_JOB);

            fuelsensor.fsEE.tank.irregular.spline.node_counter = 0;//fix

            //no es necesario resetear todos los vectores cuando se ingresa una nueva tabla, suficiente con el 1 elemento,
            //la consistencia de no poder ing. un valor < al anterior es valida
            fuelsensor.fsEE.tank.irregular.spline.node[0].X = 0.0f;
            fuelsensor.fsEE.tank.irregular.spline.node[0].A = 0.0f;
            break;

//        case 2:menu_setSubMenu();
//        break;

//        default:
//            thismenu_sm0 = 0;//hasta q haga el menu de modif.
//            break;
    }
}

static inline void mFuelSensor_irregular_menu_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_TANK_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_irregular_menu_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irregular_menu_fxKey0, mFuelSensor_irregular_menu_fxKey1, mFuelSensor_irregular_menu_fxKey2,
    mFuelSensor_irregular_menu_fxKey3, mFuelSensor_irregular_menu_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
void mFuelSensor_irregular_menu_job(void)
{
#define MENU_FUELSENSOR_IRREGULAR_MAINMENU 3
    String db_mFuelSensor_irregular_menu[MENU_FUELSENSOR_IRREGULAR_MAINMENU] =
    {
        str_clearPrint("1] UNITS OF TABLE", 0),
        str_clearPrint("2] NEW TABLE", 0),
        str_clearPrint("3] MODIFY TABLE", 0),
    };

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irregular_menu_fxKey);

        vscroll.init(db_mFuelSensor_irregular_menu, MENU_FUELSENSOR_IRREGULAR_MAINMENU, dispShowBuff, 3, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        lcd.clear();
        sysPrint_lcdPrintPSTR(LCD_ROW_0, 0, PSTR("[IRREGULAR TANK]"));

        thismenu_sm0 =-1;
    }

    vscroll.set_db(db_mFuelSensor_irregular_menu);//x local db
    vscroll.job();
    lcd_show_disp(LCD_ROW_1);
}

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

static inline void mFuelSensor_irregular_unitsOfTable_fxKey0(void)
{
}
static inline void mFuelSensor_irregular_unitsOfTable_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mFuelSensor_irregular_unitsOfTable_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mFuelSensor_irregular_unitsOfTable_fxKey3(void)
{
    vscroll.stack_push();
    vscroll.base_offset_reset();

    switch (vscroll.get_markPosc())
    {
        case 0:
            menu_setSubMenu(MGO_MFUELSENSOR_IRREGULAR_UNITSOFTABLE_LENGTHUNITS_JOB);
            break;

        case 1:
            menu_setSubMenu(MGO_MFUELSENSOR_IRREGULAR_UNITSOFTABLE_VOLUMENUNITS_JOB);
            break;
    }
}
static inline void mFuelSensor_irregular_unitsOfTable_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_IRREGULAR_MENU_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_irregular_unitsOfTable_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irregular_unitsOfTable_fxKey0, mFuelSensor_irregular_unitsOfTable_fxKey1, mFuelSensor_irregular_unitsOfTable_fxKey2,
    mFuelSensor_irregular_unitsOfTable_fxKey3, mFuelSensor_irregular_unitsOfTable_fxKey4
};

//-----------------------------------------------------------------------------------------------------------


void mFuelSensor_irregular_unitsOfTable_job(void)
{
    char buf[15];
    String db_mFuelSensor_irregular_unitsOfTable[2];

    for (uint8_t i=0; i<2; i++)
    {
        fuelsensor_get_irregular_unitsOfTable_name(i, buf);
        db_mFuelSensor_irregular_unitsOfTable[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 5);
    }

    if (thismenu_sm0 == 0)//only at enter
    {
        kb_change_keyDo_pgm(mFuelSensor_irregular_unitsOfTable_fxKey);

        vscroll.init(db_mFuelSensor_irregular_unitsOfTable, 2, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);
        //
        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[UNITS OF NODE]"));
        //
        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_irregular_unitsOfTable);
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

static void mFuelSensor_irregular_unitsOfTable_lengthUnits_printUnits(void)
{
    char buf[10];
    fuelsensor_get_irregular_unitsOfTable_lengthUnits_name(fuelsensor.fsEE.tank.irregular.spline.node_units.length.units, buf);
    lcd.printAtPosc(1, 9, String(buf));
}

static inline void mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey0(void)
{
}
static inline void mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey1(void)
{
    //vscroll.sign_up();
}
static inline void mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey2(void)
{
    //vscroll.sign_down();
}
static inline void mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey3(void)
{
}
static inline void mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_IRREGULAR_UNITSOFTABLE_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey0, mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey1, mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey2,
    mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey3, mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey4
};

//-----------------------------------------------------------------------------------------------------------

void mFuelSensor_irregular_unitsOfTable_lengthUnits_job(void)
{
    char buf[MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM];

    String db_mFuelSensor_irregular_unitsOfTable_lengthUnits[MENU_FUELSENSOR_LENGTHUNITS_NUMITEM];

    for (uint8_t i=0; i<MENU_FUELSENSOR_LENGTHUNITS_NUMITEM; i++)
    {
        fuelsensor_get_irregular_unitsOfTable_lengthUnits_name(i, buf);
        db_mFuelSensor_irregular_unitsOfTable_lengthUnits[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 2);
    }

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irregular_unitsOfTable_lengthUnits_fxKey);

        vscroll.init(db_mFuelSensor_irregular_unitsOfTable_lengthUnits, MENU_FUELSENSOR_LENGTHUNITS_NUMITEM, dispShowBuff, 1, FEAT_MARK_ON);

        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[UNITS OF LENGTH]"));
        sysPrint_lcdPrintPSTR(1, 0, PSTR("Current: "));

        mFuelSensor_irregular_unitsOfTable_lengthUnits_printUnits();
        //
        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_irregular_unitsOfTable_lengthUnits);
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
static void mFuelSensor_irregular_unitsOfTable_volumenUnits_printVolume(void)
{
    char buf[10];
    fuelsensor_get_irregular_unitsOfTable_volumenUnits_name(fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units, buf);
    lcd.printAtPosc(1, 9, String(buf)+" ");
}

static inline void mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey0(void)
{
}
static inline void mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey1(void)
{
    vscroll.sign_up();

}
static inline void mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units = posc;

    fuelsensor_savedata();

    mFuelSensor_irregular_unitsOfTable_volumenUnits_printVolume();

}
static inline void mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_IRREGULAR_UNITSOFTABLE_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey0, mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey1, mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey2,
    mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey3, mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey4
};

//-----------------------------------------------------------------------------------------------------------

void mFuelSensor_irregular_unitsOfTable_volumenUnits_job(void)
{
    char buf[MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM];

    String db_mFuelSensor_irregular_unitsOfTable_volumenUnits[MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM];

    for (uint8_t i=0; i<MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM; i++)
    {
        fuelsensor_get_irregular_unitsOfTable_volumenUnits_name(i, buf);
        db_mFuelSensor_irregular_unitsOfTable_volumenUnits[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 2);
    }

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irregular_unitsOfTable_volumenUnits_fxKey);
        //
        vscroll.init(db_mFuelSensor_irregular_unitsOfTable_volumenUnits, MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM, dispShowBuff, 2, FEAT_MARK_ON);
        //
        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[UNITS OF VOLUME]"));
        sysPrint_lcdPrintPSTR(1, 0, PSTR("Current: "));

        mFuelSensor_irregular_unitsOfTable_volumenUnits_printVolume();
        //
        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_irregular_unitsOfTable_volumenUnits);
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

static inline void mFuelSensor_irregularVol_newTable_fxKey0(void)
{
}
static inline void mFuelSensor_irregularVol_newTable_fxKey1(void)
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

static inline void mFuelSensor_irregularVol_newTable_fxKey2(void)
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

static inline void mFuelSensor_irregularVol_newTable_fxKey3(void)
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

static inline void mFuelSensor_irregularVol_newTable_fxKey4(void)
{
    uint8_t node_counter = fuelsensor.fsEE.tank.irregular.spline.node_counter;

    //1.- check consistency

    //taskman_fuelSensor.sm3 = 5; //set default -> Error


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

    if ( (setnum.bitflag & 0x1E) != 0)//ERRORS
    {
        menu_setSubMenu(MGO_MFUELSENSOR_IRREGULARVOL_NEWTABLE_ERRORCONSISTENCY_JOB);
    }
    //------------------------------------------------------------------------------
    else if ( (setnum.bitflag & 0x1E) == 0)//NO ERRORS
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

            //taskman_fuelSensor.sm3 = 6;
            menu_setSubMenu(MGO_MFUELSENSOR_IRREGULARVOL_NEWTABLE_SAVE_CONTINUE_JOB);

            thismenu_sm1 = 0;
        }
    }

}

PTRFX_retVOID const mFuelSensor_irregularVol_newTable_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irregularVol_newTable_fxKey0, mFuelSensor_irregularVol_newTable_fxKey1, mFuelSensor_irregularVol_newTable_fxKey2,
    mFuelSensor_irregularVol_newTable_fxKey3, mFuelSensor_irregularVol_newTable_fxKey4
};

static void mFuelSensor_irregularVol_newTable_fill_spline_nodes(void);

void mFuelSensor_irregularVol_newTable_job(void)//New Table
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irregularVol_newTable_fxKey);

        thismenu_sm1 = 0x0;
        thismenu_sm0 = -1;

//        Xstr[PARAM_STR_LENGTH]="000.00" ;
//        Astr[PARAM_STR_LENGTH]="0000.00";

        //fuelsensor.fsEE.tank.irregular.spline.node_counter = 0;//ESTO RESETEABA node_counter! -> antes de entrar al menu!
    }

    if (thismenu_sm1 == 0)
    {
        lcd.clear();

        sysPrint_lcdPrintPSTR(0, 0, PSTR("[NODE-TABLE]"));
        //sysPrint_lcdPrintPSTR(2, 3, PSTR("[cm]  , [gallons]"));
        sysPrint_lcdPrintPSTR(2, 3, PSTR("[cm]  , "));
        char buf[10];
        fuelsensor_get_irregular_unitsOfTable_volumenUnits_name(fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units, buf);
        lcd.printAtPosc(2, 11, "["+String(buf)+"]");
        lcd.printAtPosc(3, 9, ",");



        //carga lo que haya grabado
//        for (uint8_t i=0; i< SPLINE_NODES_MAX; i++)
//        {
//            fuelsensor.fsEE.tank.irregular.spline.node[i].X = 0.0f;
//            fuelsensor.fsEE.tank.irregular.spline.node[i].A = 0.0f;
//        }
        //fuelsensor_savedata();

        thismenu_sm1++;
    }
    if (thismenu_sm1 == 1)
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

        thismenu_sm1 = -1;
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

static void mFuelSensor_irregularVol_newTable_errorConsistency_fxKey0(void)
{
    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_errorConsistency.opt == 0)
    {
        thismenu_sm0++;//mFuelSensor_irregularVol_newTable_errorConsistency.opt++;//skip the error
        thismenu_sm1 = 0;
    }
}
static void mFuelSensor_irregularVol_newTable_errorConsistency_fxKey1(void)
{
    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_errorConsistency.opt == 0)
    {
        thismenu_sm0++;//mFuelSensor_irregularVol_newTable_errorConsistency.opt++;//skip the error
        thismenu_sm1 = 0;
    }
    else
    {
        vscroll.sign_up();
    }
}
static void mFuelSensor_irregularVol_newTable_errorConsistency_fxKey2(void)
{
    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_errorConsistency.opt == 0)
    {
        thismenu_sm0++;//mFuelSensor_irregularVol_newTable_errorConsistency.opt++;//skip the error
        thismenu_sm1 = 0;
    }
    else
    {
        vscroll.sign_down();
    }
}
static void mFuelSensor_irregularVol_newTable_errorConsistency_fxKey3(void)
{
    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_errorConsistency.opt == 0)
    {
        //"[ERROR]"));
        //msg+="[Length] ";
        //msg+="[Volume]";
        //"must be different"));
        //"to zero"));
        //"must be greater"));
        //"than previous node"

        thismenu_sm0++;//mFuelSensor_irregularVol_newTable_errorConsistency.opt++;//skip the error
        thismenu_sm1 = 0;
    }
    else
    {
        //"[ERROR]"));
        //"Discard & continue"
        //"entering new node?"
        //"1] Yes"
        //"2] No"

        uint8_t posc = vscroll.get_markPosc();

        if (posc == 0)
        {
            thismenu_sm1 = 0;

            menu_setSubMenu(MGO_MFUELSENSOR_IRREGULARVOL_NEWTABLE_JOB);
            //vscroll.stack_pop();
        }
        else
        {
            //check minimum
            //check min
            if (fuelsensor.fsEE.tank.irregular.spline.node_counter < SPLINE_NODES_MIN)
            {
                //taskman_fuelSensor.sm3 = 7;//3;//show minNode
                //taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;
                menu_setSubMenu(MGO_MFUELSENSOR_IRREGULARVOL_NEWTABLE_MINNODE_JOB);

            }
            else
            {

                //exit to menu "IRREGULAR TANK"
                menu_setSubMenu(MGO_MFUELSENSOR_IRREGULAR_MENU_JOB);
                vscroll.stack_pop();
            }
        }
    }
}
static void mFuelSensor_irregularVol_newTable_errorConsistency_fxKey4(void)
{
    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_errorConsistency.opt == 0)
    {
        thismenu_sm0++;//mFuelSensor_irregularVol_newTable_errorConsistency.opt++;//skip the error
        thismenu_sm1 = 0;
    }
    else
    {

    }
}

PTRFX_retVOID const mFuelSensor_irregularVol_newTable_errorConsistency_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irregularVol_newTable_errorConsistency_fxKey0, mFuelSensor_irregularVol_newTable_errorConsistency_fxKey1, mFuelSensor_irregularVol_newTable_errorConsistency_fxKey2,
    mFuelSensor_irregularVol_newTable_errorConsistency_fxKey3, mFuelSensor_irregularVol_newTable_errorConsistency_fxKey4
};

void mFuelSensor_irregularVol_newTable_errorConsistency_job(void)
{
    uint8_t node_counter = fuelsensor.fsEE.tank.irregular.spline.node_counter;

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irregularVol_newTable_errorConsistency_fxKey);

        lcd.clear();

        sysPrint_lcdPrintPSTR(LCD_ROW_0, 0, PSTR("[ERROR]"));
        //
        //mFuelSensor_irregularVol_newTable_errorConsistency.opt = 0x00;
        thismenu_sm1 = 0;
        //
        //thismenu_sm0 = -1;
        thismenu_sm0++;
    }

    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_errorConsistency.opt == 0)
    {
        if (thismenu_sm1 == 0)
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
            thismenu_sm1 = -1;
        }
    }
    else
    {
        String db_vscroll[2] =
        {
            str_clearPrint("1] Yes", 7),
            str_clearPrint("2] No", 7),
        };

        if (thismenu_sm1 == 0)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("Discard & continue"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("entering new node?"));

            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, 0x00, 0x00);

            thismenu_sm1 = -1;
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        lcd_show_disp(LCD_ROW_2);
    }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
static void mFuelSensor_irregularVol_newTable_save_continue_fxKey0(void)
{
}
static void mFuelSensor_irregularVol_newTable_save_continue_fxKey1(void)
{
    vscroll.sign_up();
}
static void mFuelSensor_irregularVol_newTable_save_continue_fxKey2(void)
{
    vscroll.sign_down();
}
static void mFuelSensor_irregularVol_newTable_save_continue_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_save_continue.opt == 0)//step 0
    {
        //"Save node?"
        //"1] Yes"
        //"2] No"
        if (posc == 0)//Yes
        {
            fuelsensor.fsEE.tank.irregular.spline.node_counter++;

            if (fuelsensor.fsEE.tank.irregular.spline.node_counter == (SPLINE_NODES_MAX))
            {
//                taskman_fuelSensor.sm3 = 8;//visualizar maximum...
//                taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;
                menu_setSubMenu(MGO_MFUELSENSOR_IRREGULARVOL_NEWTABLE_MAXIMUM_SIZE_JOB);
            }
            else
            {
                thismenu_sm1 = 0;
                thismenu_sm0++;//mFuelSensor_irregularVol_newTable_save_continue.opt++;//next step
            }
        }
        else//No
        {
            if (fuelsensor.fsEE.tank.irregular.spline.node_counter < SPLINE_NODES_MIN)
            {
//                taskman_fuelSensor.sm3 = 7;//show minNode
//                taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;
                menu_setSubMenu(MGO_MFUELSENSOR_IRREGULARVOL_NEWTABLE_MINNODE_JOB);
            }
            else
            {
                thismenu_sm1 = 0;
                thismenu_sm0++;//mFuelSensor_irregularVol_newTable_save_continue.opt++;
            }
        }
    }
    else//step 1
    {
        //"Continue entering"
        //"new node?"
        if (posc == 0)
        {
            menu_setSubMenu(MGO_MFUELSENSOR_IRREGULARVOL_NEWTABLE_JOB);
        }
        else
        {
            //check min
            if (fuelsensor.fsEE.tank.irregular.spline.node_counter < SPLINE_NODES_MIN)
            {

                menu_setSubMenu(MGO_MFUELSENSOR_IRREGULARVOL_NEWTABLE_MINNODE_JOB);
            }
            else
            {
                //exit to menu "IRREGULAR TANK"

                menu_setSubMenu(MGO_MFUELSENSOR_IRREGULARVOL_NEWTABLE_JOB);
                vscroll.stack_pop();
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
static void mFuelSensor_irregularVol_newTable_save_continue_fxKey4(void)
{
}

PTRFX_retVOID const mFuelSensor_irregularVol_newTable_save_continue_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irregularVol_newTable_save_continue_fxKey0, mFuelSensor_irregularVol_newTable_save_continue_fxKey1, mFuelSensor_irregularVol_newTable_save_continue_fxKey2,
    mFuelSensor_irregularVol_newTable_save_continue_fxKey3, mFuelSensor_irregularVol_newTable_save_continue_fxKey4
};

void mFuelSensor_irregularVol_newTable_save_continue_job(void)
{
    if (thismenu_sm0 == 0)
    {

        kb_change_keyDo_pgm(mFuelSensor_irregularVol_newTable_save_continue_fxKey);

        //mFuelSensor_irregularVol_newTable_save_continue.opt = 0;
        thismenu_sm1 = 0;

        //thismenu_sm0 = -1;
        thismenu_sm0++;
    }

    String db_vscroll[2] =
    {
        str_clearPrint("1] Yes", 7),
        str_clearPrint("2] No", 7),
    };

    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_save_continue.opt == 0)
    {
        if (thismenu_sm1 == 0)
        {
            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("Save node?"));

            thismenu_sm1 = -1;
        }
    }
    else
    {
        if (thismenu_sm1 == 0)
        {
            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

            lcd.clear();

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("Continue entering"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("new node?"));//then EXIT

            thismenu_sm1 = -1;
        }
    }
    vscroll.set_db(db_vscroll);
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}

////////////////////////////////////////////////////////////////////////
static void mFuelSensor_irregularVol_newTable_minNode_fxKey0(void)
{
    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_minNode.opt == 0)
    {
        thismenu_sm0++;//mFuelSensor_irregularVol_newTable_minNode.opt++;//skip the warning
        thismenu_sm1 = 0;
    }
}
static void mFuelSensor_irregularVol_newTable_minNode_fxKey1(void)
{
    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_minNode.opt == 0)
    {
        thismenu_sm0++;//mFuelSensor_irregularVol_newTable_minNode.opt++;//skip the warning
        thismenu_sm1 = 0;
    }
    else
    {
        vscroll.sign_up();
    }
}
static void mFuelSensor_irregularVol_newTable_minNode_fxKey2(void)
{
    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_minNode.opt == 0)
    {
        thismenu_sm0++;//mFuelSensor_irregularVol_newTable_minNode.opt++;//skip the warning
        thismenu_sm1 = 0;
    }
    else
    {
        vscroll.sign_down();
    }
}
static void mFuelSensor_irregularVol_newTable_minNode_fxKey3(void)
{
    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_minNode.opt == 0)
    {
        //"[WARNING]"));
        //"Number of node is"));
        //"lower than "+ String(SPLINE_NODES_MIN));
        //"(Press any key...)")

        thismenu_sm0++;//mFuelSensor_irregularVol_newTable_minNode.opt++;//skip the warning
        thismenu_sm1 = 0;
    }
    else
    {
        //"1] Yes", 7),
        //"2] No", 7),
        //"Exit & discard the"));
        //"node-table anyway?"));

        uint8_t posc = vscroll.get_markPosc();

        if (posc == 0)
        {

            //exit to menu "IRREGULAR TANK"

            menu_setSubMenu(MGO_MFUELSENSOR_IRREGULAR_MENU_JOB);
            vscroll.stack_pop();
        }
        else
        {

            menu_setSubMenu(MGO_MFUELSENSOR_IRREGULARVOL_NEWTABLE_JOB);
        }
    }
}
static void mFuelSensor_irregularVol_newTable_minNode_fxKey4(void)
{
    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_minNode.opt == 0)
    {
        thismenu_sm0++;//mFuelSensor_irregularVol_newTable_minNode.opt++;//skip the warning
        thismenu_sm1 = 0;
    }
    else
    {

    }
}

PTRFX_retVOID const mFuelSensor_irregularVol_newTable_minNode_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irregularVol_newTable_minNode_fxKey0, mFuelSensor_irregularVol_newTable_minNode_fxKey1, mFuelSensor_irregularVol_newTable_minNode_fxKey2,
    mFuelSensor_irregularVol_newTable_minNode_fxKey3, mFuelSensor_irregularVol_newTable_minNode_fxKey4
};

void mFuelSensor_irregularVol_newTable_minNode_job(void)
{
    uint8_t node_counter = fuelsensor.fsEE.tank.irregular.spline.node_counter;

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irregularVol_newTable_minNode_fxKey);

        //
        //mFuelSensor_irregularVol_newTable_minNode.opt = 0x00;
        thismenu_sm1 = 0;
        //
        //thismenu_sm0 = -1;
        thismenu_sm0++;
    }

    if (thismenu_sm0 == 1)//(mFuelSensor_irregularVol_newTable_minNode.opt == 0)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[WARNING]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Number of node is"));
            sysPrint_lcdPrint(LCD_ROW_2, STR_CENTER, "lower than "+ String(SPLINE_NODES_MIN));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("(Press any key...)"));

            thismenu_sm1 = -1;
        }
    }
    else
    {
        String db_vscroll[2] =
        {
            str_clearPrint("1] Yes", 7),
            str_clearPrint("2] No", 7),
        };

        if (thismenu_sm1 == 0)
        {
            lcd.clear();

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("Exit & discard the"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("node-table anyway?"));

            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

            thismenu_sm1 = -1;
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        lcd_show_disp(LCD_ROW_2);
    }
}

////////////////////////////////////////////////////////////////////////
void _escape(void)
{
    //exit to menu "IRREGULAR TANK"
    menu_setSubMenu(MGO_MFUELSENSOR_IRREGULAR_MENU_JOB);
    vscroll.stack_pop();
}
static void mFuelSensor_irregularVol_newTable_maximum_size_fxKey0(void)
{
    _escape();
}
static void mFuelSensor_irregularVol_newTable_maximum_size_fxKey1(void)
{
    _escape();
}
static void mFuelSensor_irregularVol_newTable_maximum_size_fxKey2(void)
{
    _escape();
}
static void mFuelSensor_irregularVol_newTable_maximum_size_fxKey3(void)
{
    _escape();
}
static void mFuelSensor_irregularVol_newTable_maximum_size_fxKey4(void)
{
    _escape();
}

PTRFX_retVOID const mFuelSensor_irregularVol_newTable_maximum_size_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irregularVol_newTable_maximum_size_fxKey0, mFuelSensor_irregularVol_newTable_maximum_size_fxKey1, mFuelSensor_irregularVol_newTable_maximum_size_fxKey2,
    mFuelSensor_irregularVol_newTable_maximum_size_fxKey3, mFuelSensor_irregularVol_newTable_maximum_size_fxKey4
};

void mFuelSensor_irregularVol_newTable_maximum_size_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irregularVol_newTable_maximum_size_fxKey);

        lcd.clear();

        sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("Node-table has"));
        sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("reached maximum"));
        sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("size. Press any"));
        sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("key to exit..."));

        thismenu_sm0 = -1;
    }
}

