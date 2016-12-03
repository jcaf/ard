/****************************************************************************
* Project: Battery System
*
* 2016-05-24-16.04
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

#include "setnum.h"
//#define DTOSTRF_SIGN_POS 1
//#define PARAM_STR_LENGTH (7+1)
char PARAM0str[7+1];//="0000.00";
char PARAM1str[7+1];//="0000.00";
char PARAM2str[7+1];//="0000.00";

struct _setnum setnum;

void setnum_digit_blink_set2blank(void)
{
    setnum.digit.blink.sm0 = 1;
    setnum.digit.blink.bf.toggle = BLINK_TOGGLE_BLANK;
}

void setnum_digit_blink_set2noblank(void)
{
    setnum.digit.blink.sm0 = 1;
    setnum.digit.blink.bf.toggle = BLINK_TOGGLE_NOBLANK;
}
