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

struct _menuFuelSensor_warning
{
    int sm0;
    int sm1;
}menuFuelSensor_warning;

static inline void menuFuelSensor_warning_fxKey0(void)
{
}
static inline void menuFuelSensor_warning_fxKey1(void)
{
    if (--menuFuelSensor_warning.sm1 < 0)
        menuFuelSensor_warning.sm1 = 0;
    else
        menuFuelSensor_warning.sm0 = 1;
}
static inline void menuFuelSensor_warning_fxKey2(void)
{
    if (++menuFuelSensor_warning.sm1 > 1)
    {
        menuFuelSensor_warning.sm1 = 0;

        taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;

        taskman_fuelSensor.sm0 =1;
    }
    else
        menuFuelSensor_warning.sm0 = 1;
}
static inline void menuFuelSensor_warning_fxKey3(void)
{
}
static inline void menuFuelSensor_warning_fxKey4(void)
{
}

PTRFX_retVOID const menuFuelSensor_warning_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_warning_fxKey0, menuFuelSensor_warning_fxKey1, menuFuelSensor_warning_fxKey2,
    menuFuelSensor_warning_fxKey3, menuFuelSensor_warning_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
static void menuFuelSensor_warning_job(void)
{
    if (menuFuelSensor_warning.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_warning_fxKey);

        menuFuelSensor_warning.sm1 = 0;
        menuFuelSensor_warning.sm0++;
    }

    if (menuFuelSensor_warning.sm0 == 1)
    {
        if (menuFuelSensor_warning.sm1 == 0)
        {
            sysPrint_lcdPrintPSTR(0, STR_CENTER, PSTR("[WARNING]"));
            sysPrint_lcdPrintPSTR(1, STR_CENTER, PSTR("This sensor use"));
            sysPrint_lcdPrintPSTR(2, STR_CENTER, PSTR("diesel, biodisel,"));
            sysPrint_lcdPrintPSTR(3, STR_CENTER, PSTR("kerosene, gasoline"));
        }

        else if (menuFuelSensor_warning.sm1 == 1)
        {
            sysPrint_lcdPrintPSTR(0, STR_CENTER, PSTR("Do not measure"));
            sysPrint_lcdPrintPSTR(1, STR_CENTER, PSTR("conductive material"));
            sysPrint_lcdPrintPSTR(2, STR_CENTER, PSTR("like water, etc."));
            sysPrint_lcdPrintPSTR(3, STR_CENTER, PSTR("(Press any key...)"));
        }

        menuFuelSensor_warning.sm0 = -1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
VScroll VS_menuFuelSensor_mainMenu;

struct _menuFuelSensor_mainMenu
{
    int8_t sm0;
    //int8_t sm1;
}menuFuelSensor_mainMenu;


static inline void menuFuelSensor_mainMenu_fxKey0(void)
{
}
static inline void menuFuelSensor_mainMenu_fxKey1(void)
{
    VS_menuFuelSensor_mainMenu.sign_up();
}
static inline void menuFuelSensor_mainMenu_fxKey2(void)
{
    VS_menuFuelSensor_mainMenu.sign_down();
}

static inline void menuFuelSensor_mainMenu_fxKey3(void)
{
    uint8_t posc = VS_menuFuelSensor_mainMenu.get_markPosc();

    taskman_fuelSensor.sm1 = posc+1;
    taskman_fuelSensor.sign = TASKMAN_SIGN_ENTER;

    menuFuelSensor_mainMenu.sm0 = 1;
}
static inline void menuFuelSensor_mainMenu_fxKey4(void)
{
    taskman_mainMenu.sm0 = 0;
    taskman_fuelSensor.sm0 = 0;
    kb_stackPop();
    kb_stackPop();
}

PTRFX_retVOID const menuFuelSensor_mainMenu_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menuFuelSensor_mainMenu_fxKey0, menuFuelSensor_mainMenu_fxKey1, menuFuelSensor_mainMenu_fxKey2,
    menuFuelSensor_mainMenu_fxKey3, menuFuelSensor_mainMenu_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
static void menuFuelSensor_mainMenu_job(void)
{
#define MENU_FUELSENSOR_OPTIONS 4

    String db_menuFuelSensor_mainMenu[MENU_FUELSENSOR_OPTIONS] =
    {
        str_clearPrint("1] MODEL OF SENSOR", 0),
        str_clearPrint("2] CALIBRATION", 0),
        str_clearPrint("3] FUEL TANK", 0),
        str_clearPrint("4] OUTPUT TYPE", 0),
    };

    if (menuFuelSensor_mainMenu.sm0 == 0)//siempre SOLO cuando se entra!!!!
    {
        kb_stackPush();
        kb_change_keyDo_pgm(menuFuelSensor_mainMenu_fxKey);

        VS_menuFuelSensor_mainMenu.init(db_menuFuelSensor_mainMenu, MENU_FUELSENSOR_OPTIONS, dispShowBuff, 2, FEAT_MARK_ON);

        menuFuelSensor_mainMenu.sm0++;
    }

    if (menuFuelSensor_mainMenu.sm0 == 1)
    {
        lcd.clear();
        sysPrint_lcdPrintPSTR(LCD_ROW_0, 0, PSTR("[FUEL SENSOR]"));

        menuFuelSensor_mainMenu.sm0 =-1;
    }


    sysPrint_lcdPrint(LCD_ROW_1, 0, fuelsensor_reading());

    VS_menuFuelSensor_mainMenu.set_db(db_menuFuelSensor_mainMenu);//x local db
    VS_menuFuelSensor_mainMenu.job();
    lcd_show_disp(LCD_ROW_2);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
struct _taskman_fuelSensor taskman_fuelSensor;

void mainMenu_fuelSensor_job(void)
{
    if (taskman_fuelSensor.sign == TASKMAN_SIGN_ENTER)
    {
        taskman_fuelSensor.reInitJob = 1;
    }

    if (taskman_fuelSensor.sm0 == 0)
    {
        if (taskman_fuelSensor.reInitJob)
            menuFuelSensor_warning.sm0 = 0;

        menuFuelSensor_warning_job();
    }

    if (taskman_fuelSensor.sm0 == 1)
    {
        if (taskman_fuelSensor.sm1 == 0)
        {
            if (taskman_fuelSensor.reInitJob)
                menuFuelSensor_mainMenu.sm0 = 0;

            menuFuelSensor_mainMenu_job();
        }
        ////////////////// MODEL OF SENSOR ///////////////
        if (taskman_fuelSensor.sm1 == 1)
        {
            if (taskman_fuelSensor.reInitJob)
                menuFuelSensor_modelOfsensor.sm0 = 0;

            menuFuelSensor_modelOfsensor_job();
        }
        ////////////////// CALIBRATION /////////////////
        if (taskman_fuelSensor.sm1 == 2)
        {
            if (taskman_fuelSensor.sm2 == 0)
            {
                if (taskman_fuelSensor.reInitJob)
                    menuFuelSensor_calibration.sm0 = 0;

                menuFuelSensor_calibration_job();
            }
            if (taskman_fuelSensor.sm2 == 1)
            {
                //do Full/Zero calibration
                if (taskman_fuelSensor.reInitJob)
                    menuFuelSensor_lengthTankDepth_FullZeroCalibration.sm0 = 0;

                menuFuelSensor_lengthTankDepth_FullZeroCalibration_job();

            }
            if (taskman_fuelSensor.sm2 == 2)
            {
                if (taskman_fuelSensor.sm3 == 0)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_lengthTankDepth.sm0 = 0;

                    menuFuelSensor_lengthTankDepth_job();
                }

                if (taskman_fuelSensor.sm3 == 1)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_lengthTankDepth_errorConsistency.sm0 = 0;

                    menuFuelSensor_lengthTankDepth_errorConsistency_job();

                }

                if (taskman_fuelSensor.sm3 == 2)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_lengthTankDepth_noErrorConsistency.sm0 = 0;

                    menuFuelSensor_lengthTankDepth_noErrorConsistency_job();
                }
            }
        }

        ///////////////////// FUEL TANK ///////////////////
        if (taskman_fuelSensor.sm1 == 3)//4)
        {
            if (taskman_fuelSensor.sm2 == 0)
            {
                if (taskman_fuelSensor.reInitJob)
                    menuFuelSensor_tank.sm0 = 0;

                menuFuelSensor_tank_job();//menu fuel tank
            }

            if (taskman_fuelSensor.sm2 == 1)
            {
                if (taskman_fuelSensor.reInitJob)
                    menuFuelSensor_typeOfTank.sm0 = 0;

                menuFuelSensor_typeOfTank_job();
            }

            if (taskman_fuelSensor.sm2 == 2)
            {
                if (taskman_fuelSensor.sm3 == 0)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_rectangularVolume.sm0 = 0;

                    menuFuelSensor_rectangularVolume_job();//fill length  x width
                }
                if (taskman_fuelSensor.sm3 == 1)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_rectangularVolumen_save_and_exit.sm0 = 0;

                    menuFuelSensor_rectangularVolumen_save_and_exit_job();
                }
            }
            if (taskman_fuelSensor.sm2 == 3)
            {
                if (taskman_fuelSensor.sm3 == 0)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_irregular_mainMenu.sm0 = 0;

                    menuFuelSensor_irregular_mainMenu_job();
                }
                if (taskman_fuelSensor.sm3 == 1)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_irregular_unitsOfTable.sm0 = 0;

                    menuFuelSensor_irregular_unitsOfTable_job();
                }
                if (taskman_fuelSensor.sm3 == 2)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_irregular_unitsOfTable_lengthUnits.sm0 = 0;

                    menuFuelSensor_irregular_unitsOfTable_lengthUnits_job();
                }
                if (taskman_fuelSensor.sm3 == 3)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_irregular_unitsOfTable_volumenUnits.sm0 = 0;

                    menuFuelSensor_irregular_unitsOfTable_volumenUnits_job();
                }
                if (taskman_fuelSensor.sm3 == 4)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_irregularVolume.sm0 = 0;

                    menuFuelSensor_irregularVolume_job();
                }
                if (taskman_fuelSensor.sm3 == 5)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_irregularVolume_errorConsistency.sm0 = 0;

                    menuFuelSensor_irregularVolume_errorConsistency_job();
                }
                if (taskman_fuelSensor.sm3 == 6)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_irregularVolume_save_continue.sm0 = 0;

                    menuFuelSensor_irregularVolume_save_continue_job();
                }
                if (taskman_fuelSensor.sm3 == 7)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_irregularVolume_minNode.sm0 = 0;

                    menuFuelSensor_irregularVolume_minNode_job();
                }
                if (taskman_fuelSensor.sm3 == 8)
                {
                    if (taskman_fuelSensor.reInitJob)
                        menuFuelSensor_irregularVolume_maximum_size.sm0 = 0;

                    menuFuelSensor_irregularVolume_maximum_size_job();
                }

            }
        }

        ///////////////// OUTPUT TYPE //////////////////
        if (taskman_fuelSensor.sm1 == 4)//3)
        {
            if (taskman_fuelSensor.sm2 == 0)
            {
                if (taskman_fuelSensor.reInitJob)
                    menuFuelSensor_outputType.sm0 = 0;

                menuFuelSensor_outputType_job();
            }
            if (taskman_fuelSensor.sm2 == 1)
            {
                if (taskman_fuelSensor.reInitJob)
                    menuFuelSensor_lengthUnits.sm0 = 0;

                menuFuelSensor_lengthUnits_job();
            }
            if (taskman_fuelSensor.sm2 == 2)
            {
                if (taskman_fuelSensor.reInitJob)
                    menuFuelSensor_volumenUnits.sm0 = 0;

                menuFuelSensor_volumenUnits_job();
            }
        }
    }

    //
    if(taskman_fuelSensor.reInitJob == 1)
        taskman_fuelSensor.reInitJob = 0;

    if (taskman_fuelSensor.sign == TASKMAN_SIGN_ENTER)
        taskman_fuelSensor.sign = TASKMAN_SIGN_CLEAR;
}
