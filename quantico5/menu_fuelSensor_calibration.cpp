/****************************************************************************
* Project: Battery System
*
* 2016-05-24-15.52
* jcaf @ jcafOpenSuse
*
* Copyright 2013 Juan Carlos Aguero Flores
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

#include "menu_fuelSensor_calibration.h"
#include "setnum.h"

#include "leds.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mFuelSensor_calibration_fxKey0(void)
{
}
static inline void mFuelSensor_calibration_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mFuelSensor_calibration_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void mFuelSensor_calibration_fxKey3(void)
{
    switch (vscroll.get_markPosc())
    {
        case 0:
            menu_setSubMenu(MGO_MFUELSENSOR_FULLZEROCALIBRATION_JOB);
            fuelsensor.bf.do_fullzero = FS_DO_FULL_CALIB;
            break;

        case 1:
            menu_setSubMenu(MGO_MFUELSENSOR_FULLZEROCALIBRATION_JOB);
            fuelsensor.bf.do_fullzero = FS_DO_ZERO_CALIB;
            break;

        case 2:
            menu_setSubMenu(MGO_MFUELSENSOR_LENGTHTANKDEPTH_JOB);
            break;
    }
    //++---
    vscroll.stack_push();
    vscroll.base_offset_reset();
    //++---
}
static inline void mFuelSensor_calibration_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setSubMenu(MGO_MFUELSENSOR_MENU_JOB);
}

PTRFX_retVOID const mFuelSensor_calibration_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_calibration_fxKey0, mFuelSensor_calibration_fxKey1, mFuelSensor_calibration_fxKey2,
    mFuelSensor_calibration_fxKey3, mFuelSensor_calibration_fxKey4
};

void mFuelSensor_calibration_job(void)
{
#define MENU_FUELSENSOR_CALIBRATION 3

    String db_mFuelSensor_calibration[MENU_FUELSENSOR_CALIBRATION]=
    {
        str_clearPrint("1]SET FULL SCALE", 0),
        str_clearPrint("2]SET ZERO", 0),
        str_clearPrint("3]LENGTH TANK DEPTH", 0),
    };

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_calibration_fxKey);

        vscroll.init(db_mFuelSensor_calibration, MENU_FUELSENSOR_CALIBRATION, dispShowBuff, 3, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        sysPrint_lcdPrintPSTR(0, 0, PSTR("[CALIBRATION]"));

        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_calibration);
    vscroll.job();
    lcd_show_disp();
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
inline static void mFuelSensor_FullZeroCalibration_escape(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_CALIBRATION_JOB);
    vscroll.stack_pop();
}

static void mFuelSensor_FullZeroCalibration_fxKey0(void)
{
    thismenu_sm1 = 0;

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
    else if (thismenu_sm0 == 4)
    {
        mFuelSensor_FullZeroCalibration_escape();

    }
    else if (thismenu_sm0 == 5)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 6)
    {
    }
}
static void mFuelSensor_FullZeroCalibration_fxKey1(void)
{
    thismenu_sm1 = 0;

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
    else if (thismenu_sm0 == 4)
    {
        mFuelSensor_FullZeroCalibration_escape();

    }
    else if (thismenu_sm0 == 5)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 6)
    {
        vscroll.sign_up();
    }

}
static void mFuelSensor_FullZeroCalibration_fxKey2(void)
{
    thismenu_sm1 = 0;

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
    else if (thismenu_sm0 == 4)
    {
        mFuelSensor_FullZeroCalibration_escape();
    }
    else if (thismenu_sm0 == 5)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 6)
    {
        vscroll.sign_down();
    }
}
static void mFuelSensor_FullZeroCalibration_fxKey3(void)
{
    uint8_t opt = vscroll.get_markPosc();
    thismenu_sm1 = 0;

    if (thismenu_sm0 == 1)//attention
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)//waiting
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 3)//do calib
    {
    }
    else if (thismenu_sm0 == 4)//success.
    {
        mFuelSensor_FullZeroCalibration_escape();
    }
    else if (thismenu_sm0 == 5)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 6)//retry
    {
        if (opt == 0)
            thismenu_sm0 = 2;
        else
            mFuelSensor_FullZeroCalibration_escape();
    }
    else if (thismenu_sm0 == 7)//cancel calibration?
    {
        if (opt == 0)
        {
            digitalWrite(LED1, LOW);

            mFuelSensor_FullZeroCalibration_escape();
        }
        else
            thismenu_sm0 = thismenu_sm0_last;//restore last menu
    }

}
static void mFuelSensor_FullZeroCalibration_fxKey4(void)
{
    thismenu_sm1 = 0;

    if (thismenu_sm0 == 1)//ATTENTION warning
    {
        thismenu_sm0_last = thismenu_sm0;
        thismenu_sm0 = 7;//go escape
    }
    else if (thismenu_sm0 == 2)//"Press key <Enter> to")
    {
        thismenu_sm0_last = thismenu_sm0;
        thismenu_sm0 = 7;//go escape

        lcd.noBlink();
    }
    else if (thismenu_sm0 == 3)
    {

    }
    else if (thismenu_sm0 == 4)
    {
        mFuelSensor_FullZeroCalibration_escape();
    }
    else if (thismenu_sm0 == 5)
    {
        thismenu_sm1 = 0;
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 6)
    {
    }
    else if (thismenu_sm0 == 7)
    {
    }
}

PTRFX_retVOID const mFuelSensor_FullZeroCalibration_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_FullZeroCalibration_fxKey0, mFuelSensor_FullZeroCalibration_fxKey1, mFuelSensor_FullZeroCalibration_fxKey2,
    mFuelSensor_FullZeroCalibration_fxKey3, mFuelSensor_FullZeroCalibration_fxKey4
};

void mFuelSensor_FullZeroCalibration_job(void)
{
    String msg;


    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_FullZeroCalibration_fxKey);
        thismenu_sm0++;
        thismenu_sm1 = 0;
    }

    if (thismenu_sm0 == 1)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[ATTENTION]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("You have to do Full"));
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("calibration firstly,"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("then Zero calib..."));

            thismenu_sm1 = -1;
        }
    }

    if (thismenu_sm0 == 2)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[MESSAGE]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Press key <Enter> to"));
            msg = "begin ";
            if (fuelsensor.bf.do_fullzero == FS_DO_FULL_CALIB)
                msg+= "Full";
            else
                msg+= "Zero";
            sysPrint_lcdPrint(LCD_ROW_2, STR_CENTER, msg);
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("calibration...."));

            thismenu_sm1 = -1;
        }

        //-------------------------------------
        /*
        B: The character indicated by the cursor blinks when B is 1 (Figure 13). The blinking is displayed as
        switching between all blank dots and displayed characters at a speed of 409.6-ms intervals when fcp or fOSC
        is 250 kHz. The cursor and blinking can be set to display simultaneously. (The blinking frequency changes
        according to fOSC or the reciprocal of fcp. For example, when fcp is 270 kHz, 409.6 � 250/270 = 379.2 ms.)
        */
        lcd.setCursor(17,4);
        lcd.blink();

        if (millis() - thismenu_last_millis0 >= 409)
        {
            thismenu_last_millis0 = millis();
            TogglingBit(PORTWxLED1,PINxLED1);
        }
        //-------------------------------------
    }


    if (thismenu_sm0 == 3)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.noBlink();

            lcd.clear();

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[MESSAGE]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Doing calibration"));
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("...."));

            thismenu_sm1 = -1;

            digitalWrite(LED1, HIGH);
        }

        uint8_t calib_codret;

        if (fuelsensor.bf.do_fullzero == FS_DO_FULL_CALIB)
        {
            calib_codret = jt606_send_fullrangeAdj(JT606_CMD_SET);

            if (calib_codret)
            {
                fuelsensor.fsEE.bf.full_calib = 1;
                fuelsensor.fsEE.bf.zero_calib = 0;

                fuelsensor_check_consistency();
                fuelsensor_savedata();
            }
        }
        else
        {
            calib_codret = jt606_send_zeroAdj(JT606_CMD_SET);

            if (calib_codret)
            {
                if (fuelsensor.fsEE.bf.full_calib == 1)
                {
                    fuelsensor.fsEE.bf.zero_calib = 1;

                    fuelsensor_check_consistency();
                    fuelsensor_savedata();
                }
            }
        }

        digitalWrite(LED1, LOW);

        if (calib_codret)
        {
            thismenu_sm0 = 4;
        }
        else
        {
            thismenu_sm0 = 5;
        }

        thismenu_sm1 = 0;
    }

    if (thismenu_sm0 == 4)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[MESSAGE]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Calibration was"));
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("successfully!"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("(Press any key...)"));
            //delay(500);
            thismenu_sm1 = -1;
        }
    }
    if (thismenu_sm0 == 5)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[ERROR]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Calibration failed!"));
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("Sensor need to be"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("re-calibrated!"));

            thismenu_sm1 = -1;
        }
    }

    if ( (thismenu_sm0 == 6) || (thismenu_sm0 == 7) )
    {
        String db_vscroll[2] =
        {
            str_clearPrint("1] Yes", 7),
            str_clearPrint("2] No", 7),
        };

        if (thismenu_sm1 == 0)
        {
            lcd.clear();

            if (thismenu_sm0 == 6)
            {
                sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[MESSAGE]"));
                sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Retry calibration?"));
            }
            else if (thismenu_sm0 == 7)
            {
                sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[MESSAGE]"));
                sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Cancel calibration?"));
            }
            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, 0x00, 0x00);

            thismenu_sm1 = -1;
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        lcd_show_disp(LCD_ROW_2);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SETNUM_PARAM_INNERTANK 0
#define SETNUM_PARAM_ZERO2FULL  1
#define SETNUM_PARAM_FULL2UPPER 2

static void lengthTankDepth_setnum_digit_blink_print(uint8_t blink_toggle, uint8_t setnum_digit_position );

static void lengthTankDepth_setnum_check_boundary(void);

static inline void mFuelSensor_lengthTankDepth_fxKey0(void)
{
}
static inline void mFuelSensor_lengthTankDepth_fxKey1(void)
{
    char *PARAMstr;

    if (setnum.param == SETNUM_PARAM_INNERTANK)
        PARAMstr = PARAM0str;
    else if (setnum.param == SETNUM_PARAM_ZERO2FULL)
        PARAMstr = PARAM1str;
    else if (setnum.param == SETNUM_PARAM_FULL2UPPER)
        PARAMstr = PARAM2str;

    if (--PARAMstr[setnum.digit.position]< '0')
        PARAMstr[setnum.digit.position] = '9';

    setnum_digit_blink_set2noblank();
}

static inline void mFuelSensor_lengthTankDepth_fxKey2(void)
{
    char *PARAMstr;

    if (setnum.param == SETNUM_PARAM_INNERTANK)
        PARAMstr = PARAM0str;
    else if (setnum.param == SETNUM_PARAM_ZERO2FULL)
        PARAMstr = PARAM1str;
    else if (setnum.param == SETNUM_PARAM_FULL2UPPER)
        PARAMstr = PARAM2str;

    if (++PARAMstr[setnum.digit.position] > '9')
        PARAMstr[setnum.digit.position] = '0';

    setnum_digit_blink_set2noblank();
}

static inline void mFuelSensor_lengthTankDepth_fxKey3(void)
{
    lengthTankDepth_setnum_check_boundary();

    lengthTankDepth_setnum_digit_blink_print(BLINK_TOGGLE_NOBLANK, setnum.digit.position);//fix latest digit before to leave

    setnum.digit.position++;

    if (setnum.digit.position == PARAM_STR_DIG_POS_DP)//skip decimal point
        setnum.digit.position++;

    if (setnum.digit.position > PARAM_STR_DIG_POS_MAX)//
    {
        //lengthTankDepth_setnum_digit_blink_print(BLINK_TOGGLE_NOBLANK, PARAM_STR_DIG_POS_MAX);//fix latest digit before to leave

        setnum.digit.position = 0;

        if (++setnum.param == 3)
            setnum.param = 0;

        //help title
        if (setnum.param == 0)
            sysPrint_lcdPrintPSTR(0, 0, PSTR("[D1 in cm]"));
        else if (setnum.param == 1)
            sysPrint_lcdPrintPSTR(0, 0, PSTR("[D2 in cm]"));
        else if (setnum.param == 2)
            sysPrint_lcdPrintPSTR(0, 0, PSTR("[D3 in cm]"));
        //
    }

    setnum_digit_blink_set2blank();
}

static inline void mFuelSensor_lengthTankDepth_fxKey4(void)
{
    float innertank = strtod(PARAM0str, (char**)0);
    float  zero2full = strtod(PARAM1str, (char**)0);
    //float  full2upper =strtod(PARAM2str, (char**)0);

    if ( ( innertank == 0.0f) || ( zero2full == 0.0f) )
    {
        //taskman_fuelSensor.sm3 = 1;//error consistency
        menu_setSubMenu(MGO_MFUELSENSOR_LENGTHTANKDEPTH_ERRORCONSISTENCY_JOB);
    }
    else
    {
        //taskman_fuelSensor.sm3 = 2;//no error
        menu_setSubMenu(MGO_MFUELSENSOR_LENGTHTANKDEPTH_NOERRORCONSISTENCY_JOB);
    }

    //thismenu_sm0 = 0;//enter to new fx
}

PTRFX_retVOID const mFuelSensor_lengthTankDepth_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_lengthTankDepth_fxKey0, mFuelSensor_lengthTankDepth_fxKey1, mFuelSensor_lengthTankDepth_fxKey2,
    mFuelSensor_lengthTankDepth_fxKey3, mFuelSensor_lengthTankDepth_fxKey4
};

static void print_PARAM0(float f);
static void print_PARAM1(float f);
static void print_PARAM2(float f);

void mFuelSensor_lengthTankDepth_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_lengthTankDepth_fxKey);

        thismenu_sm0++;
    }

    if (thismenu_sm0 == 1)
    {
        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[D1 in cm]"));
        sysPrint_lcdPrintPSTR(1, 0, PSTR("Inner Tank ="));//[HEIGHTS IN CM]
        sysPrint_lcdPrintPSTR(2, 0, PSTR("Zero->Full ="));//[EFFEC.LEN.SCAL.CAL]
        sysPrint_lcdPrintPSTR(3, 0, PSTR("Full->Upper="));//[LEN.TO UPPER SURF.]

        print_PARAM0(fuelsensor.fsEE.calib.tank.innertank);
        print_PARAM1(fuelsensor.fsEE.calib.tank.zero2full);
        print_PARAM2(fuelsensor.fsEE.calib.tank.full2upper);
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
        lengthTankDepth_setnum_digit_blink_print(setnum.digit.blink.bf.toggle, setnum.digit.position);
        //
        setnum.digit.blink.last_millis = millis();
        setnum.digit.blink.sm0 = 0;
    }
}

static void print_PARAM0(float f)
{
    dtostrf(f, PARAM_STR_LENGTH-DTOSTRF_SIGN_POS, PARAM_STR_DP_PRECISION, PARAM0str);
    dtostrf_fill_zero(PARAM0str);
    lcd.printAtPosc(LCD_ROW_1,13, String(PARAM0str));
}
static void print_PARAM1(float f)
{
    dtostrf(f, PARAM_STR_LENGTH-DTOSTRF_SIGN_POS, PARAM_STR_DP_PRECISION, PARAM1str);
    dtostrf_fill_zero(PARAM1str);
    lcd.printAtPosc(LCD_ROW_2,13, String(PARAM1str));
}
static void print_PARAM2(float f)
{
    dtostrf(f, PARAM_STR_LENGTH-DTOSTRF_SIGN_POS, PARAM_STR_DP_PRECISION, PARAM2str);
    dtostrf_fill_zero(PARAM2str);
    lcd.printAtPosc(LCD_ROW_3,13, String(PARAM2str));
}
static void lengthTankDepth_setnum_digit_blink_print(uint8_t blink_toggle, uint8_t setnum_digit_position )
{
    uint8_t col, fil;
    char param_digit_str=' ';
    char *PARAMstr;

    col=13;
    if (setnum.param == SETNUM_PARAM_INNERTANK)
    {
        fil=1;
        PARAMstr = PARAM0str;
    }
    else if (setnum.param == SETNUM_PARAM_ZERO2FULL)
    {
        fil=2;
        PARAMstr = PARAM1str;
    }
    else if (setnum.param == SETNUM_PARAM_FULL2UPPER)
    {
        fil=3;
        PARAMstr = PARAM2str;
    }

    if (blink_toggle == BLINK_TOGGLE_NOBLANK)
        param_digit_str = PARAMstr[setnum_digit_position];

    col += setnum_digit_position;
    lcd.printAtPosc(fil, col,String(param_digit_str));
}

static void lengthTankDepth_setnum_check_boundary(void)
{
    float d1,d2,d3;
    //float limit;

    d1 = strtod(PARAM0str, (char**)0);

    //check for d2
    d2 = strtod(PARAM1str, (char**)0);

    if (d2> d1)
    {
        d2= d1;
        print_PARAM1(d2);
    }

    //check for d3
    d3 = strtod(PARAM2str, (char**)0);

    if ( (d2+d3) > d1)
    {
        d3 = d1-d2;
        print_PARAM2(d3);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void mFuelSensor_lengthTankDepth_returnToItsCaller(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_CALIBRATION_JOB);
    vscroll.stack_pop();
}

static void mFuelSensor_lengthTankDepth_errorConsistency_fxKey0(void)
{
    if (thismenu_sm1 == 0)
    {
        thismenu_sm1++;//skip the error
        thismenu_sm0 = 1;
    }
    else if (thismenu_sm1 == 1)
    {
    }
    else if (thismenu_sm1 == 2)
    {
        mFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}
static void mFuelSensor_lengthTankDepth_errorConsistency_fxKey1(void)
{
    if (thismenu_sm1 == 0)
    {
        thismenu_sm1++;//skip the error
        thismenu_sm0 = 1;
    }
    else if (thismenu_sm1 == 1)
    {
        vscroll.sign_up();
    }
    else if (thismenu_sm1 == 2)
    {
        mFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}
static void mFuelSensor_lengthTankDepth_errorConsistency_fxKey2(void)
{
    if (thismenu_sm1 == 0)
    {
        thismenu_sm1++;//skip the error
        thismenu_sm0 = 1;
    }
    else if (thismenu_sm1 == 1)
    {
        vscroll.sign_down();
    }
    else if (thismenu_sm1 == 2)
    {
        mFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}
static void mFuelSensor_lengthTankDepth_errorConsistency_fxKey3(void)
{
    if (thismenu_sm1 == 0)
    {
        thismenu_sm1++;//skip the error
        thismenu_sm0 = 1;
    }
    else if (thismenu_sm1 == 1)
    {
        uint8_t posc = vscroll.get_markPosc();

        //"[DATA INCONSISTENT]"
        //"Fix the errors?"
        if (posc == 0)
        {

            thismenu_sm0 = 1;

            fuelsensor.fsEE.calib.tank.innertank = strtod(PARAM0str, (char**)0);
            fuelsensor.fsEE.calib.tank.zero2full = strtod(PARAM1str, (char**)0);
            fuelsensor.fsEE.calib.tank.full2upper= strtod(PARAM2str, (char**)0);

            //thismenu_sm0 = 0;
            menu_setSubMenu(MGO_MFUELSENSOR_LENGTHTANKDEPTH_JOB);
        }
        else
        {
            fuelsensor.fsEE.bf.lengthsTankdepth_consistent = 0;

            //-------------- reset to default
            fuelsensor.fsEE.outputType.type = (FS_OUTPUTTYPE)LENGTH;
            fuelsensor.fsEE.outputType.length.units = FS_UNITMEA_LENGTH_PERCENTAGE;
            //--------------

            fuelsensor_savedata();

            thismenu_sm0 = 1;
            thismenu_sm1++;
        }
    }
    else if (thismenu_sm1 == 2)
    {
        //"[WARNING]"
        //"Data are wrong"
        //"to get volume"
        //"(Press any key...)"
        mFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}

static void mFuelSensor_lengthTankDepth_errorConsistency_fxKey4(void)
{
    if (thismenu_sm1 == 0)
    {
        thismenu_sm1++;//skip the error
        thismenu_sm0 = 1;
    }
    else if (thismenu_sm1 == 1)
    {
    }
    else if (thismenu_sm1 == 2)
    {
        mFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}

PTRFX_retVOID const mFuelSensor_lengthTankDepth_errorConsistency_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_lengthTankDepth_errorConsistency_fxKey0, mFuelSensor_lengthTankDepth_errorConsistency_fxKey1, mFuelSensor_lengthTankDepth_errorConsistency_fxKey2,
    mFuelSensor_lengthTankDepth_errorConsistency_fxKey3, mFuelSensor_lengthTankDepth_errorConsistency_fxKey4
};

void mFuelSensor_lengthTankDepth_errorConsistency_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_lengthTankDepth_errorConsistency_fxKey);
        thismenu_sm1 = 0x00;
        thismenu_sm0++;
    }

    if (thismenu_sm1 == 0)
    {
        if (thismenu_sm0 == 1)
        {
            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[ERROR]"));

            float innertank = strtod(PARAM0str, (char**)0);
            float  zero2full = strtod(PARAM1str, (char**)0);

            String msg="Length ";

            if ( innertank == 0)
                msg+="[D1] ";

            if ( zero2full == 0)
                msg+="[D2]";

            sysPrint_lcdPrint(LCD_ROW_1, STR_CENTER, msg);
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("must be greater"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("than zero"));

            thismenu_sm0 = -1;
        }
    }
    else if (thismenu_sm1 == 1)
    {
        String db_vscroll[2] =
        {
            str_clearPrint("1] Yes", 7),
            str_clearPrint("2] No", 7),
        };

        if (thismenu_sm0 == 1)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[DATA INCONSISTENT]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Fix the errors?"));

            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);

            thismenu_sm0 = -1;
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        lcd_show_disp(LCD_ROW_2);
    }
    else if (thismenu_sm1 == 2)
    {
        if (thismenu_sm0 == 1)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[WARNING]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Data are wrong"));
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("to get volume"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("(Press any key...)"));

            thismenu_sm0 = -1;
        }
    }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

static void mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey0(void)
{
}
static void mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey1(void)
{
    vscroll.sign_up();
}
static void mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey2(void)
{
    vscroll.sign_down();
}
static void mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    if (thismenu_sm0 == 1)
    {
        //[DATA IS CONSISTENT]
        //    "Save data?
        if (posc == 0)//Yes
        {
            //save data to EEPROM
            fuelsensor.fsEE.bf.lengthsTankdepth_consistent = 1;

            fuelsensor.fsEE.calib.tank.innertank = strtod(PARAM0str, (char**)0);
            fuelsensor.fsEE.calib.tank.zero2full = strtod(PARAM1str, (char**)0);
            fuelsensor.fsEE.calib.tank.full2upper= strtod(PARAM2str, (char**)0);

            fuelsensor_savedata();
        }
        // else//nothing to do for "NO" option
        // {
        // }

        thismenu_sm1 = 0;
        thismenu_sm0++;//next step
    }
    else if (thismenu_sm0 == 2)
    {
        //"[MESSAGE]"
        //"Exit ?"
        if (posc == 0)
        {
            mFuelSensor_lengthTankDepth_returnToItsCaller();
        }
        else
        {
            thismenu_sm0 = 0;//1;

            menu_setSubMenu(MGO_MFUELSENSOR_LENGTHTANKDEPTH_JOB);

        }
    }
}
static void mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey4(void)
{
}

PTRFX_retVOID const mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey0, mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey1, mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey2,
    mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey3, mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey4
};

void mFuelSensor_lengthTankDepth_noErrorConsistency_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey);

        thismenu_sm1 = 0;

        thismenu_sm0++;
    }

    String db_vscroll[2] =
    {
        str_clearPrint("1] Yes", 7),
        str_clearPrint("2] No", 7),
    };

    if (thismenu_sm0 == 1)
    {
        if (thismenu_sm1 == 0)
        {
            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);

            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[DATA IS CONSISTENT]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Save data?"));

            thismenu_sm1 = -1;
        }
    }
    else//thismenu_sm0 == 2
    {
        if (thismenu_sm1 == 0)
        {
            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

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

