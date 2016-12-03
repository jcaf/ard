/****************************************************************************
* Project: Battery System
*
* 2016-05-24-15.52
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

#include "menu_fuelSensor_calibration.h"
#include "setnum.h"

//#include <DigitalToggle.h>
#include "leds.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
VScroll VS_menuFuelSensor_calibration;

static inline void menuFuelSensor_calibration_fxKey0(void)
{
}
static inline void menuFuelSensor_calibration_fxKey1(void)
{
    VS_menuFuelSensor_calibration.sign_up();
}
static inline void menuFuelSensor_calibration_fxKey2(void)
{
    VS_menuFuelSensor_calibration.sign_down();
}

static inline void menuFuelSensor_calibration_fxKey3(void)
{
    taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;

    uint8_t opt = VS_menuFuelSensor_calibration.get_markPosc();

    taskman_fuelSensor.sm2 = 1;

    if (opt == 0)
        fuelsensor.bf.do_fullzero = FS_DO_FULL_CALIB;
    else if (opt == 1)
        fuelsensor.bf.do_fullzero = FS_DO_ZERO_CALIB;
    else if (opt == 2)
    {
        taskman_fuelSensor.sm2 = 2;
        taskman_fuelSensor.sm3 = 0;
    }

}
static inline void menuFuelSensor_calibration_fxKey4(void)
{
    taskman_fuelSensor.sm1 = 0;
    kb_stackPop();
}

PTRFX_retVOID const menuFuelSensor_calibration_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_calibration_fxKey0, menuFuelSensor_calibration_fxKey1, menuFuelSensor_calibration_fxKey2,
    menuFuelSensor_calibration_fxKey3, menuFuelSensor_calibration_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
struct _menuFuelSensor_calibration  menuFuelSensor_calibration;

void menuFuelSensor_calibration_job(void)
{
#define MENU_FUELSENSOR_CALIBRATION 3

    String db_menuFuelSensor_calibration[MENU_FUELSENSOR_CALIBRATION]=
    {
        str_clearPrint("1]SET FULL SCALE", 0),
        str_clearPrint("2]SET ZERO", 0),
        str_clearPrint("3]LENGTH TANK DEPTH", 0),
    };

    if (menuFuelSensor_calibration.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_calibration_fxKey);

        VS_menuFuelSensor_calibration.init(db_menuFuelSensor_calibration, MENU_FUELSENSOR_CALIBRATION, dispShowBuff, 3, FEAT_MARK_ON);

        menuFuelSensor_calibration.sm0++;
    }

    if (menuFuelSensor_calibration.sm0 == 1)//la opcion de marcado conservarla
    {
        //VS_menuFuelSensor_calibration.init(db_menuFuelSensor_calibration, MENU_FUELSENSOR_CALIBRATION, dispShowBuff, 3, FEAT_MARK_ON);

        sysPrint_lcdPrintPSTR(0, 0, PSTR("[CALIBRATION]"));

        menuFuelSensor_calibration.sm0 = -1;
    }

    VS_menuFuelSensor_calibration.set_db(db_menuFuelSensor_calibration);
    VS_menuFuelSensor_calibration.job();
    lcd_show_disp();
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

struct _menuFuelSensor_lengthTankDepth_FullZeroCalibration menuFuelSensor_lengthTankDepth_FullZeroCalibration;

inline static void menuFuelSensor_lengthTankDepth_FullZeroCalibration_escape(void)
{
    //__
    taskman_fuelSensor.sm2 = 0;
    menuFuelSensor_calibration.sm0 = 1;
    kb_stackPop();
    //__
}

static void menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey0(void)
{
    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 1)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 2)
    {
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 3)
    {
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 4)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration_escape();

    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 5)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 6)
    {
        //menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
}
static void menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey1(void)
{
    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 1)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 2)
    {
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 3)
    {
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 4)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration_escape();

    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 5)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 6)
    {
        vscroll.sign_up();
    }

}
static void menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey2(void)
{
    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 1)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 2)
    {
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 3)
    {
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 4)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration_escape();
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 5)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 6)
    {
        vscroll.sign_down();
    }
}
static void menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey3(void)
{
    uint8_t opt = vscroll.get_markPosc();

    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 1)//attention
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 2)//waiting
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 3)//do calib
    {

    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 4)//success.
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration_escape();
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 5)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 6)
    {
        if (opt == 0)
        {
            //retry

            menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 = 2;
            menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
        }
        else
        {
            //escape
            menuFuelSensor_lengthTankDepth_FullZeroCalibration_escape();
        }
    }

}
static void menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey4(void)
{
    menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;

    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 1)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 2)
    {

    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 3)
    {

    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 4)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration_escape();
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 5)
    {
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
    else if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 6)
    {
        //menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }
}

PTRFX_retVOID const menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey0, menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey1, menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey2,
    menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey3, menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey4
};

void menuFuelSensor_lengthTankDepth_FullZeroCalibration_job(void)
{
    String msg;

    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_lengthTankDepth_FullZeroCalibration_fxKey);

        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;

        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0++;
    }

    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 1)
    {
        if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 == 0)
        {
            lcd.clear();

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[ATTENTION]"));

            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("You have to do Full"));
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("calibration firstly,"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("then Zero calib..."));

            menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = -1;
        }
    }

    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 2)
    {
        if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 == 0)
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


            menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = -1;
        }

        //-------------------------------------
        /*
        B: The character indicated by the cursor blinks when B is 1 (Figure 13). The blinking is displayed as
        switching between all blank dots and displayed characters at a speed of 409.6-ms intervals when fcp or fOSC
        is 250 kHz. The cursor and blinking can be set to display simultaneously. (The blinking frequency changes
        according to fOSC or the reciprocal of fcp. For example, when fcp is 270 kHz, 409.6 × 250/270 = 379.2 ms.)
        */
        lcd.setCursor(17,4);
        lcd.blink();

        static unsigned long last_millis;
        if (millis() - last_millis >= 409)
        {
            last_millis = millis();
            TogglingBit(PORTC,3);
        }
        //-------------------------------------
    }


    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 3)
    {
        if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 == 0)
        {
            lcd.noBlink();

            lcd.clear();

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[MESSAGE]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Doing calibration"));
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("...."));

            menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = -1;

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
            menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 = 4;
        }
        else
        {
            menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 = 5;
        }

        menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = 0;
    }

    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 4)
    {
        if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 == 0)
        {
            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[MESSAGE]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Calibration was"));
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("successfully!"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("(Press any key...)"));
            delay(500);
            menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = -1;
        }
    }
    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 5)
    {
        if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 == 0)
        {
            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[ERROR]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Calibration failed!"));
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("Sensor need to be"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("re-calibrated!"));

            menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = -1;
        }
    }
    if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 == 6)
    {
        String db_vscroll[2] =
        {
            str_clearPrint("1] Yes", 7),
            str_clearPrint("2] No", 7),
        };

        if (menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 == 0)
        {
            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[MESSAGE]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Retry calibration?"));

            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

            menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm1 = -1;
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        lcd_show_disp(LCD_ROW_2);
    }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _menuFuelSensor_lengthTankDepth menuFuelSensor_lengthTankDepth;

#define SETNUM_PARAM_INNERTANK 0
#define SETNUM_PARAM_ZERO2FULL  1
#define SETNUM_PARAM_FULL2UPPER 2

static void lengthTankDepth_setnum_digit_blink_print(uint8_t blink_toggle, uint8_t setnum_digit_position );

static void lengthTankDepth_setnum_check_boundary(void);

static inline void menuFuelSensor_lengthTankDepth_fxKey0(void)
{
}
static inline void menuFuelSensor_lengthTankDepth_fxKey1(void)
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

static inline void menuFuelSensor_lengthTankDepth_fxKey2(void)
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

static inline void menuFuelSensor_lengthTankDepth_fxKey3(void)
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

static inline void menuFuelSensor_lengthTankDepth_fxKey4(void)
{
    float innertank = strtod(PARAM0str, (char**)0);
    float  zero2full = strtod(PARAM1str, (char**)0);
    //float  full2upper =strtod(PARAM2str, (char**)0);

    taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;

    if ( ( innertank == 0.0f) || ( zero2full == 0.0f) )
    {
        taskman_fuelSensor.sm3 = 1;
    }
    else
    {
        taskman_fuelSensor.sm3 = 2;
    }
}

PTRFX_retVOID const menuFuelSensor_lengthTankDepth_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_lengthTankDepth_fxKey0, menuFuelSensor_lengthTankDepth_fxKey1, menuFuelSensor_lengthTankDepth_fxKey2,
    menuFuelSensor_lengthTankDepth_fxKey3, menuFuelSensor_lengthTankDepth_fxKey4
};

static void print_PARAM0(float f);
static void print_PARAM1(float f);
static void print_PARAM2(float f);

void menuFuelSensor_lengthTankDepth_job(void)
{
    if (menuFuelSensor_lengthTankDepth.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_lengthTankDepth_fxKey);

        menuFuelSensor_lengthTankDepth.sm0++;
    }

    if (menuFuelSensor_lengthTankDepth.sm0 == 1)
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
        menuFuelSensor_lengthTankDepth.sm0 = -1;
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
struct _menuFuelSensor_lengthTankDepth_errorConsistency menuFuelSensor_lengthTankDepth_errorConsistency;

static void menuFuelSensor_lengthTankDepth_returnToItsCaller(void)
{
    menuFuelSensor_calibration.sm0 = 1;
    taskman_fuelSensor.sm2 = 0;
    kb_stackPop();
    kb_stackPop();
}

static void menuFuelSensor_lengthTankDepth_errorConsistency_fxKey0(void)
{
    if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 0)
    {
        menuFuelSensor_lengthTankDepth_errorConsistency.sm1++;//skip the error
        menuFuelSensor_lengthTankDepth_errorConsistency.sm0 = 1;
    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 1)
    {
    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 2)
    {
        menuFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}
static void menuFuelSensor_lengthTankDepth_errorConsistency_fxKey1(void)
{
    if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 0)
    {
        menuFuelSensor_lengthTankDepth_errorConsistency.sm1++;//skip the error
        menuFuelSensor_lengthTankDepth_errorConsistency.sm0 = 1;
    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 1)
    {
        vscroll.sign_up();
    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 2)
    {
        menuFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}
static void menuFuelSensor_lengthTankDepth_errorConsistency_fxKey2(void)
{
    if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 0)
    {
        menuFuelSensor_lengthTankDepth_errorConsistency.sm1++;//skip the error
        menuFuelSensor_lengthTankDepth_errorConsistency.sm0 = 1;
    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 1)
    {
        vscroll.sign_down();
    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 2)
    {
        menuFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}
static void menuFuelSensor_lengthTankDepth_errorConsistency_fxKey3(void)
{
    if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 0)
    {
        menuFuelSensor_lengthTankDepth_errorConsistency.sm1++;//skip the error
        menuFuelSensor_lengthTankDepth_errorConsistency.sm0 = 1;
    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 1)
    {
        uint8_t posc = vscroll.get_markPosc();

        if (posc == 0)
        {
            taskman_fuelSensor.sm3 = 0;
            menuFuelSensor_lengthTankDepth.sm0 = 1;

            fuelsensor.fsEE.calib.tank.innertank = strtod(PARAM0str, (char**)0);
            fuelsensor.fsEE.calib.tank.zero2full = strtod(PARAM1str, (char**)0);
            fuelsensor.fsEE.calib.tank.full2upper= strtod(PARAM2str, (char**)0);

            kb_stackPop();
        }
        else
        {
            fuelsensor.fsEE.bf.lengthsTankdepth_consistent = 0;
            //-------------- reset to default
            fuelsensor.fsEE.outputType.type = (FS_OUTPUTTYPE)LENGTH;
            fuelsensor.fsEE.outputType.length.units = FS_UNITMEA_LENGTH_PERCENTAGE;

            //--------------
            fuelsensor_savedata();

            menuFuelSensor_lengthTankDepth_errorConsistency.sm0 = 1;
            menuFuelSensor_lengthTankDepth_errorConsistency.sm1++;
        }
    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 2)
    {
        menuFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}

static void menuFuelSensor_lengthTankDepth_errorConsistency_fxKey4(void)
{
    if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 0)
    {
        menuFuelSensor_lengthTankDepth_errorConsistency.sm1++;//skip the error
        menuFuelSensor_lengthTankDepth_errorConsistency.sm0 = 1;
    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 1)
    {

    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 2)
    {
        menuFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}

PTRFX_retVOID const menuFuelSensor_lengthTankDepth_errorConsistency_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_lengthTankDepth_errorConsistency_fxKey0, menuFuelSensor_lengthTankDepth_errorConsistency_fxKey1, menuFuelSensor_lengthTankDepth_errorConsistency_fxKey2,
    menuFuelSensor_lengthTankDepth_errorConsistency_fxKey3, menuFuelSensor_lengthTankDepth_errorConsistency_fxKey4
};

void menuFuelSensor_lengthTankDepth_errorConsistency_job(void)
{
    if (menuFuelSensor_lengthTankDepth_errorConsistency.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_lengthTankDepth_errorConsistency_fxKey);
        //
        menuFuelSensor_lengthTankDepth_errorConsistency.sm1 = 0x00;

        //
        menuFuelSensor_lengthTankDepth_errorConsistency.sm0++;
    }

    if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 0)
    {
        if (menuFuelSensor_lengthTankDepth_errorConsistency.sm0 == 1)
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

            menuFuelSensor_lengthTankDepth_errorConsistency.sm0 = -1;
        }
    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 1)
    {
        String db_vscroll[2] =
        {
            str_clearPrint("1] Yes", 7),
            str_clearPrint("2] No", 7),
        };

        if (menuFuelSensor_lengthTankDepth_errorConsistency.sm0 == 1)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[DATA INCONSISTENT]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Fix the errors?"));

            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

            menuFuelSensor_lengthTankDepth_errorConsistency.sm0 = -1;
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        lcd_show_disp(LCD_ROW_2);
    }
    else if (menuFuelSensor_lengthTankDepth_errorConsistency.sm1 == 2)
    {
        if (menuFuelSensor_lengthTankDepth_errorConsistency.sm0 == 1)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[WARNING]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Data are wrong"));
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("to get volume"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("(Press any key...)"));

            menuFuelSensor_lengthTankDepth_errorConsistency.sm0 = -1;
        }
    }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
struct _menuFuelSensor_lengthTankDepth_noErrorConsistency menuFuelSensor_lengthTankDepth_noErrorConsistency;

static void menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey0(void)
{
}
static void menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey1(void)
{
    vscroll.sign_up();
}
static void menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey2(void)
{
    vscroll.sign_down();
}
static void menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    if (menuFuelSensor_lengthTankDepth_noErrorConsistency.sm0 == 1)
    {
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

        menuFuelSensor_lengthTankDepth_noErrorConsistency.sm1 = 0;
        menuFuelSensor_lengthTankDepth_noErrorConsistency.sm0++;//next step
    }
    else if (menuFuelSensor_lengthTankDepth_noErrorConsistency.sm0 == 2)
    {
        if (posc == 0)
        {
            menuFuelSensor_lengthTankDepth_returnToItsCaller();
        }
        else
        {
            taskman_fuelSensor.sm3 = 0;
            menuFuelSensor_lengthTankDepth.sm0 = 1;
            kb_stackPop();
        }
    }
}
static void menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey4(void)
{
}

PTRFX_retVOID const menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey0, menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey1, menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey2,
    menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey3, menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey4
};

void menuFuelSensor_lengthTankDepth_noErrorConsistency_job(void)
{
    if (menuFuelSensor_lengthTankDepth_noErrorConsistency.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_lengthTankDepth_noErrorConsistency_fxKey);

        menuFuelSensor_lengthTankDepth_noErrorConsistency.sm1 = 0;

        menuFuelSensor_lengthTankDepth_noErrorConsistency.sm0++;
    }

    String db_vscroll[2] =
    {
        str_clearPrint("1] Yes", 7),
        str_clearPrint("2] No", 7),
    };

    if (menuFuelSensor_lengthTankDepth_noErrorConsistency.sm0 == 1)
    {
        if (menuFuelSensor_lengthTankDepth_noErrorConsistency.sm1 == 0)
        {
            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[DATA IS CONSISTENT]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Save data?"));

            menuFuelSensor_lengthTankDepth_noErrorConsistency.sm1 = -1;
        }
    }
    else
    {
        if (menuFuelSensor_lengthTankDepth_noErrorConsistency.sm1 == 0)
        {
            vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON);

            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[MESSAGE]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Exit ?"));//then EXIT

            menuFuelSensor_lengthTankDepth_noErrorConsistency.sm1 = -1;
        }
    }
    vscroll.set_db(db_vscroll);
    vscroll.job();
    lcd_show_disp(LCD_ROW_2);
}
