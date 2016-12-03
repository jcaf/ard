#ifndef BOOT_H_
#define BOOT_H_

    #define SDCARD_BOOT_LCD_TIME2VIEW 0//1000//0//500//1500//1.5s

    uint8_t sdcard_init(void);
    void sdcard_runtest_H10(void);
    uint8_t system_set_functionmode(void);

    int8_t mainvar_isFound(String setting_name);
    String  mainvar_getvalue(String setting_name);
    uint8_t mainvar_setvalue(String setting_name, String setting_value);


#include "sdcard.h"
#include <SD.h>
struct   _variable cfgfiles_getName_getValue(File *myFile, String * setting_name, String * setting_value);


#endif // BOOT_H_

