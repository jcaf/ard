/****************************************************************************
* Project: Battery System
*
* 2016-06-24-12.29
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

#include "menu.h"
#include "menu_inputs.h"
#include "menu_fuelSensor.h"
#include "menu_fuelSensor_modelOfSensor.h"
#include "menu_fuelSensor_calibration.h"
#include "menu_fuelSensor_fuelTank.h"
#include "menu_fuelSensor_regularVolume.h"
#include "menu_fuelSensor_irregularVolume.h"
#include "menu_fuelSensor_outputTypeUnits.h"
//
#include "menu_bkground.h"
#include "menu_stackInDetail_V.h"
#include "menu_stackInDetail_E.h"

#include "time.h"
#include "date.h"

//PTRFX_retVOID const mBkground[] PROGMEM=
PTRFX_retVOID mBkground[]=
{
mBkground_showStackSummary_job,
mBkground_insideStackSummary_job,
mBkground_stackInDetail_V_job,
mBkground_stackInDetail_E_job,
mBkground_stackInDetail_E_UnderVolt_job,
mBkground_stackInDetail_E_OverVolt_job,
mBkground_stackInDetail_E_OpenWire_job,
};

//PTRFX_retVOID const mInput[] PROGMEM=
PTRFX_retVOID mInput[]=
{
    mInput_menu_job,
    mInput_digitalInput_job,
    mInput_fuelSensor_job
};

PTRFX_retVOID mSetTime[]=
{
    mSetTime_do,
    mSetTime_save_job
};

PTRFX_retVOID mSetDate[]=
{
    mSetDate_do,
    mSetDate_save_job
};


//PTRFX_retVOID const mFuelSensor[] PROGMEM=
PTRFX_retVOID mFuelSensor[]=
{
    mFuelSensor_warning_job,
	mFuelSensor_menu_job,
	//
	mFuelSensor_modelOfsensor_job,
	//
	mFuelSensor_calibration_job,
    mFuelSensor_FullZeroCalibration_job,
    mFuelSensor_lengthTankDepth_job,
    mFuelSensor_lengthTankDepth_errorConsistency_job,
    mFuelSensor_lengthTankDepth_noErrorConsistency_job,
    //
    mFuelSensor_tank_job,
    mFuelSensor_typeOfTank_job,
    mFuelSensor_rectangularVolume_job,
    mFuelSensor_rectangularVolumen_save_and_exit_job,
    mFuelSensor_irregular_menu_job,
    mFuelSensor_irregular_unitsOfTable_job,
    mFuelSensor_irregular_unitsOfTable_lengthUnits_job,
    mFuelSensor_irregular_unitsOfTable_volumenUnits_job,
    mFuelSensor_irregularVol_newTable_job,
    mFuelSensor_irregularVol_newTable_errorConsistency_job,
    mFuelSensor_irregularVol_newTable_save_continue_job,
    mFuelSensor_irregularVol_newTable_minNode_job,
    mFuelSensor_irregularVol_newTable_maximum_size_job,
	//
	mFuelSensor_outputType_job,
    mFuelSensor_outputType_lengthUnits_job,
    mFuelSensor_outputType_volumenUnits_job,
};

PTRFX_retVOID *menu_v[MAIN_MENU_NUMMAX+1]=
//const PTRFX_retVOID *const menu_v[MAIN_MENU_NUMMAX+1] PROGMEM=
{
    mBkground,
    //
	mInput,
	(PTRFX_retVOID *)0,
	(PTRFX_retVOID *)0,
	(PTRFX_retVOID *)0,
	(PTRFX_retVOID *)0,
	mSetTime,
	mSetDate,
	mFuelSensor
};


int8_t thismenu_sm0;
int8_t thismenu_sm1;
int8_t thismenu_sm0_last;
unsigned long thismenu_last_millis0;

int8_t men=0;
int8_t men_last=men;

//int8_t submen=-1;//sub menu
int8_t submen=0;
int8_t submen_last = submen;

void menu_setMenu(int8_t m)
{
    men = m;
}
int8_t menu_getMenu(void)
{
    return men;
}

void menu_setSubMenu(int8_t m)
{
    submen = m;
}

int8_t menu_getSubMenu(void)
{
    return submen;
}

void menu_detect_changeMenu_changeSubMenu(void)
{
	if (men_last != men )
	{
		men_last = men;//a change has been produced
		thismenu_sm0 = 0;//reset
	}

	if (submen_last != submen )
	{
		submen_last =  submen;//a change has been produced
		thismenu_sm0 = 0;//reset
	}

}

