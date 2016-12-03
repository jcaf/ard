/****************************************************************************
* Project: Battery System
*
* 2016-07-01-18.47
* jcaf @ jcafOpenSuse
*
* Copyright 2013 Juan Carlos AgÃ¼ero Flores
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
#include "sdcard.h"
#include <SPI.h>
#include <SD.h>
#include "boot.h"

struct _sdcard sdcard;

//example to use:
//char setting_value[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
//	if ( SD_get_setting_value_PSTR(PSTR("config/corfact.txt"),PSTR("FACTCORR_2V"),setting_value) )
//		Serial.println(String(setting_value));


uint8_t SD_get_setting_value_PSTR(const char *filepath, const char *setting_name_toread, char *setting_value_dest)
{
    char filepath_buff[SDCARD_STR_PATH_DIR_MAX_SIZE + SDCARD_STR_NAME_FILE_MAX_SIZE];
    strcpy_P(filepath_buff, filepath);

    char setting_name_toread_buff[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
    strcpy_P(setting_name_toread_buff, setting_name_toread);

    //
    char setting_name[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
    char setting_value[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];

    uint8_t value=0;
    struct _variable variable;
    File myFile;
    //

    if (sdcard.begin)
    {
        myFile = SD.open(filepath_buff);

        if (myFile)
        {
            while (myFile.available())
            {
                variable = cfgfiles_getName_getValue(&myFile, setting_name, setting_value);

                if (variable.bf.sintax)
                {
                    if (strcmp(setting_name_toread_buff, setting_name) == 0)
                    {
                        //*setting_value_read = setting_value;
                        strcpy(setting_value_dest, setting_value);
                        
                        value = 1;
                        break;
                    }
                }
            }
            myFile.close();

        }
    }
    return value;
}

/*
uint8_t SD_get_setting_value(const char *filepath, String setting_name_toread, String *setting_value_read)
{
    String setting_name;
    String setting_value;
    uint8_t value=0;
    struct _variable variable;
    File myFile;
    char filepath_buff[25];//8<directory>+ <8.3>

    if (sdcard.begin)
    {
        strcpy_P(filepath_buff, filepath);
        myFile = SD.open(filepath_buff);

        if (myFile)
        {
            while (myFile.available())
            {
                variable = cfgfiles_getName_getValue(&myFile, &setting_name, &setting_value);

                if (variable.bf.sintax)
                {
                    if (setting_name_toread == setting_name)
                    {
                        *setting_value_read = setting_value;
                        value = 1;
                        break;
                    }
                }
            }
            myFile.close();

        }
    }
    return value;
}
*/


