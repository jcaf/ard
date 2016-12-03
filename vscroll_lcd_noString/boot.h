#ifndef BOOT_H_
#define BOOT_H_

    #define SDCARD_BOOT_LCD_TIME2VIEW 0//0//500//1500//1.5s

    uint8_t sdcard_init(void);
    void sdcard_runtest_H10(void);
    uint8_t system_set_functionmode(void);

    int8_t mainvar_isFound(const char *setting_name);
    void mainvar_getvalue(const char *setting_name, char *setting_value_dest);
    uint8_t mainvar_setvalue(const char *setting_name, const char *setting_value);


#include "sdcard.h"
#include <SD.h>

struct _variable cfgfiles_getName_getValue(File *myFile, char *setting_name_dest, char *setting_value_dest);


#endif // BOOT_H_


