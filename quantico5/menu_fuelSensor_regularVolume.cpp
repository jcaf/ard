/****************************************************************************
* Project: Battery System
*
* 2016-05-24-15.50
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

#include "menu_fuelSensor_regularVolume.h"
#include "setnum.h"
#include "menu_fuelSensor_fuelTank.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define PARAM_STR_LENGTH (6+1)//6+\0
//#define PARAM_STR_DIG_POS_DP 3
//#define PARAM_STR_DIG_POS_MAX (PARAM_STR_LENGTH-2)
//
//#define PARAM_STR_DP_PRECISION 2

#define SETNUM_NUMBER_OF_PARAMETERS 2

#define SETNUM_PARAM_LENGTH 0
#define SETNUM_PARAM_WIDTH  1

static void rectangularVolume_setnum_digit_blink_print(uint8_t blink_toggle, uint8_t setnum_digit_position );
static void rectangularVolume_setnum_check_boundary(void);

static inline void mFuelSensor_rectangularVolume_fxKey0(void)
{
}
static inline void mFuelSensor_rectangularVolume_fxKey1(void)
{
    char *PARAMstr;

    if (setnum.param == SETNUM_PARAM_LENGTH)
        PARAMstr = PARAM0str;
    else if (setnum.param == SETNUM_PARAM_WIDTH)
        PARAMstr = PARAM1str;

    if (--PARAMstr[setnum.digit.position]< '0')
        PARAMstr[setnum.digit.position] = '9';

    setnum_digit_blink_set2noblank();
}

static inline void mFuelSensor_rectangularVolume_fxKey2(void)
{
    char *PARAMstr;

    if (setnum.param == SETNUM_PARAM_LENGTH)
        PARAMstr = PARAM0str;
    else if (setnum.param == SETNUM_PARAM_WIDTH)
        PARAMstr = PARAM1str;

    if (++PARAMstr[setnum.digit.position] > '9')
        PARAMstr[setnum.digit.position] = '0';

    setnum_digit_blink_set2noblank();
}

static inline void mFuelSensor_rectangularVolume_fxKey3(void)
{
    //rectangularVolume_setnum_check_boundary();

    rectangularVolume_setnum_digit_blink_print(BLINK_TOGGLE_NOBLANK, setnum.digit.position);//fix latest digit before to leave

    setnum.digit.position++;

    if (setnum.digit.position == PARAM_STR_DIG_POS_DP)//skip decimal point
        setnum.digit.position++;

    if (setnum.digit.position > PARAM_STR_DIG_POS_MAX)//
    {
        //rectangularVolume_setnum_digit_blink_print(BLINK_TOGGLE_NOBLANK, PARAM_STR_DIG_POS_MAX);//fix latest digit before to leave

        setnum.digit.position = 0;

        if (++setnum.param == SETNUM_NUMBER_OF_PARAMETERS)//2
            setnum.param = 0;

    }

    setnum_digit_blink_set2blank();
}

static inline void mFuelSensor_rectangularVolume_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_RECTANGULARVOLUMEN_SAVE_AND_EXIT_JOB);
}

PTRFX_retVOID const mFuelSensor_rectangularVolume_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_rectangularVolume_fxKey0, mFuelSensor_rectangularVolume_fxKey1, mFuelSensor_rectangularVolume_fxKey2,
    mFuelSensor_rectangularVolume_fxKey3, mFuelSensor_rectangularVolume_fxKey4
};

static void mFuelSensor_rectangularVolume_print_PARAM0(float f);
static void mFuelSensor_rectangularVolume_print_PARAM1(float f);

void mFuelSensor_rectangularVolume_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_rectangularVolume_fxKey);

        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[Dimensions in cm]"));
        sysPrint_lcdPrintPSTR(1, 3, PSTR("Length ="));
        sysPrint_lcdPrintPSTR(2, 3, PSTR("Width  ="));

        mFuelSensor_rectangularVolume_print_PARAM0(fuelsensor.fsEE.tank.rectangular.area.length);
        mFuelSensor_rectangularVolume_print_PARAM1(fuelsensor.fsEE.tank.rectangular.area.width);

        //
        setnum.digit.position = 0;
        setnum.param = 0;
        //
        thismenu_sm0 = -1;
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
        rectangularVolume_setnum_digit_blink_print(setnum.digit.blink.bf.toggle, setnum.digit.position);
        //
        setnum.digit.blink.last_millis = millis();
        setnum.digit.blink.sm0 = 0;
    }
}

static void mFuelSensor_rectangularVolume_print_PARAM0(float f)
{
    dtostrf(f, PARAM_STR_LENGTH-DTOSTRF_SIGN_POS, PARAM_STR_DP_PRECISION, PARAM0str);
    dtostrf_fill_zero(PARAM0str);
    lcd.printAtPosc(LCD_ROW_1,13, String(PARAM0str));
}
static void mFuelSensor_rectangularVolume_print_PARAM1(float f)
{
    dtostrf(f, PARAM_STR_LENGTH-DTOSTRF_SIGN_POS, PARAM_STR_DP_PRECISION, PARAM1str);
    dtostrf_fill_zero(PARAM1str);
    lcd.printAtPosc(LCD_ROW_2,13, String(PARAM1str));
}

static void rectangularVolume_setnum_digit_blink_print(uint8_t blink_toggle, uint8_t setnum_digit_position )
{
    uint8_t col, fil;
    char param_digit_str=' ';
    char *PARAMstr;

    col=13;
    if (setnum.param == SETNUM_PARAM_LENGTH)
    {
        fil=1;
        PARAMstr = PARAM0str;
    }
    else if (setnum.param == SETNUM_PARAM_WIDTH)
    {
        fil=2;
        PARAMstr = PARAM1str;
    }

    if (blink_toggle == BLINK_TOGGLE_NOBLANK)
        param_digit_str = PARAMstr[setnum_digit_position];

    col += setnum_digit_position;
    lcd.printAtPosc(fil, col,String(param_digit_str));
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
static void mFuelSensor_rectangularVolumen_save_and_exit_fxKey0(void)
{
    if (thismenu_sm0 == 1)
    {
        thismenu_sm1 = 0;
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
    }
    else if (thismenu_sm0 == 3)
    {
    }
}
static void mFuelSensor_rectangularVolumen_save_and_exit_fxKey1(void)
{
    if (thismenu_sm0 == 1)
    {
        thismenu_sm1 = 0;
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
        vscroll.sign_up();
    }
    else if (thismenu_sm0 == 3)
    {
        vscroll.sign_up();
    }
}
static void mFuelSensor_rectangularVolumen_save_and_exit_fxKey2(void)
{
    if (thismenu_sm0 == 1)
    {
        thismenu_sm1 = 0;
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
        vscroll.sign_down();
    }
    else if (thismenu_sm0 == 3)
    {
        vscroll.sign_down();
    }
}
static void mFuelSensor_rectangularVolumen_save_and_exit_fxKey3(void)
{
    if (thismenu_sm0 == 1)
    {
        thismenu_sm1 = 0;
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
        //"[DATA INCONSISTENT]"));
        //"Save data anaway?"));
        // or
        //"[DATA IS CONSISTENT]"));
        //"Save data ?"));

        thismenu_sm1 = 0;

        if ( vscroll.get_markPosc() == 0)//Yes
        {
            fuelsensor.fsEE.tank.rectangular.area.length = strtod(PARAM0str, (char**)0);
            fuelsensor.fsEE.tank.rectangular.area.width = strtod(PARAM1str, (char**)0);


            if ( ( fuelsensor.fsEE.tank.rectangular.area.length == 0.0f) || ( fuelsensor.fsEE.tank.rectangular.area.width == 0.0f) )
            {
                fuelsensor.fsEE.bf.rectangular_area_consistent = 0;
            }
            else
            {
                fuelsensor.fsEE.bf.rectangular_area_consistent = 1;
            }
            fuelsensor_check_consistency();
            fuelsensor_savedata();
        }
        else //No
        {

        }
        thismenu_sm1 = 0;
        //vscroll.base_offset_reset();//no hay necesidad porque las opciones que vienen son de pregunta y no guardan contexto
        thismenu_sm0++;//show next menu
    }
    else if (thismenu_sm0 == 3)
    {
        //"[MESSAGE]"));
        //"Exit ?"));//then EXIT
        thismenu_sm1 = 0;

        if ( vscroll.get_markPosc() == 0)//Yes
        {
            menu_setSubMenu(MGO_MFUELSENSOR_TANK_JOB);
            vscroll.stack_pop();
        }
        else //No
        {
            menu_setSubMenu(MGO_MFUELSENSOR_RECTANGULARVOLUME_JOB);
        }
    }
}

static void mFuelSensor_rectangularVolumen_save_and_exit_fxKey4(void)
{
    if (thismenu_sm0 == 1)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
    }
    else if (thismenu_sm0 == 3)
    {
    }
}

PTRFX_retVOID const mFuelSensor_rectangularVolumen_save_and_exit_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_rectangularVolumen_save_and_exit_fxKey0, mFuelSensor_rectangularVolumen_save_and_exit_fxKey1, mFuelSensor_rectangularVolumen_save_and_exit_fxKey2,
    mFuelSensor_rectangularVolumen_save_and_exit_fxKey3, mFuelSensor_rectangularVolumen_save_and_exit_fxKey4
};

void mFuelSensor_rectangularVolumen_save_and_exit_job(void)
{
    float length;
    float  width;

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_rectangularVolumen_save_and_exit_fxKey);

        length = strtod(PARAM0str, (char**)0);
        width = strtod(PARAM1str, (char**)0);

        if ( ( length == 0.0f) || ( width == 0.0f) )
            thismenu_sm0  = 1;
        else
            thismenu_sm0  = 2;

        thismenu_sm1 = 0;
    }

    if (thismenu_sm0 == 1)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[WARNING]"));

            String msg;

            if ( length == 0.0f)
                msg+="[Length] ";

            if ( width == 0.0f)
                msg+="[Width]";

            sysPrint_lcdPrint(LCD_ROW_1, STR_CENTER, msg);
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("must be > 0"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("to get volume"));

            thismenu_sm1 = -1;
        }
    }

    if (thismenu_sm0 >= 2)
    {
        String db_vscroll[2] =
        {
            str_clearPrint("1] Yes", 7),
            str_clearPrint("2] No", 7),
        };

        if (thismenu_sm0 == 2)
        {
            if (thismenu_sm1 == 0)
            {
                //vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);//base offset from Tank job
                vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, 0x00, 0x00);//without context push/pop

                lcd.clear();

                length = strtod(PARAM0str, (char**)0);
                width = strtod(PARAM1str, (char**)0);

                if ( ( length == 0.0f) || ( width == 0.0f) )
                {
                    sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[DATA INCONSISTENT]"));
                    sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Save data anaway?"));
                }
                else
                {
                    sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[DATA IS CONSISTENT]"));
                    sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Save data ?"));
                }
                thismenu_sm1 = -1;
            }
        }

        if (thismenu_sm0 == 3)
        {
            if (thismenu_sm1 == 0)
            {
                //vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);
                vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, 0x00, 0x00);//without context push/pop

                lcd.clear();
                sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[MESSAGE]"));
                sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Exit ?"));//then EXIT

                thismenu_sm1 = -1;
            }
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        lcd_show_disp(LCD_ROW_2);
    }
}

