/****************************************************************************
* Project: Battery System
*
* 2016-05-11-18.04
* jcaf @ jcafOpenSuse
*
* Copyright 2016 Juan Carlos Agüero Flores
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
#include "DS3231.h"
#include "rtcc.h"



struct _rtcc rtcc;

void rtc_job(void)
{
    uint8_t S10,S;
    uint8_t M10,M;
    uint8_t H10,H;

    uint8_t D10,D;
    uint8_t MM10,MM;
    uint8_t Y10,Y;

    uint8_t I2Ccfx_UserBuff[7];

    DS3231_ReadTKRegs2Array(DS3231_SECONDS_REG, I2Ccfx_UserBuff, 7);

    S10= I2Ccfx_UserBuff[0]>>4;
    S	= I2Ccfx_UserBuff[0] & 0x0F;
    rtcc.sec = S10*10 + S;


    M10= I2Ccfx_UserBuff[1]>>4;
    M	= I2Ccfx_UserBuff[1] & 0x0F;
    rtcc.min = M10*10 + M;

    H10= (I2Ccfx_UserBuff[2]>>4) & 0x01; //clear 12/24 AM/PM
    H	 = I2Ccfx_UserBuff[2] & 0x0F;
    rtcc.hour = H10*10 + H;

    rtcc.ampm = (I2Ccfx_UserBuff[2] & (1<<_DS3231_AMPM_) ) >>_DS3231_AMPM_;

    //Day 1..7
    rtcc.day = I2Ccfx_UserBuff[3];

    //Date Dd
    D10 = I2Ccfx_UserBuff[4]>>4;
    D = I2Ccfx_UserBuff[4] & 0x0F;
    rtcc.date = D10*10 + D;

    //Month Mm //01..12
    MM10 = (I2Ccfx_UserBuff[5]>>4) & 0x01;
    MM = I2Ccfx_UserBuff[5] & 0x0F;
    rtcc.month = MM10*10 + MM;

    //Year 00..99
    Y10 = (I2Ccfx_UserBuff[6]>>4) & 0x01;
    Y = I2Ccfx_UserBuff[6] & 0x0F;
    rtcc.year = Y10*10 + Y;

    //Serial.print(HH);Serial.print(":");Serial.print(MM);Serial.print(":");Serial.println(SS);
    //delay(1000);
}
