#include <Arduino.h>
#include "system.h"
#include "errorsys.h"
#include "sysprint.h"

#define ERRORSYS_STR_CODE_MAX_SIZE (6+1)
#define ERRORSYS_STR_DESC_MAX_SIZE 45

const
struct _errorsys_db
{
    const char code[ERRORSYS_STR_CODE_MAX_SIZE];
    const char desc[ERRORSYS_STR_DESC_MAX_SIZE];
    //};
    //const struct _errorsys_db errorsys_db[] PROGMEM=
} errorsys_db[] PROGMEM=
{
    //SDCARD
    {"H10.1","Check if the SDCARD is attached"},
    {"H10.2","Wrong format. Must be FAT16 or FAT32"},
    {"H10.3","File not Found"},
    {"H10.4","Variable Not Found"},
    {"H10.5","SdCard out of memory"},

    //ETHERNET: H11
    {"H11.1","No ethernet connection"},
    {"H11.2","Server not reached"},

    //POWER SOURCE: H12
    {"H12.1","Main power not available"},
    {"H12.2","Main power out of range"},

    //STACKS & CELLS: H01
    {"H01.1","Open wire"},
    {"H01.2","Cells out of range"},
    {"H01.3","High Temperature"},
    {"H01.4","High Voltage"},
    {"H01.5","Low Voltage"},
    {"H01.6","High Current"},

    //FUEL LEVEL: H02
    {"H02.1","Fuel sensor not connected"},
    {"H02.2","Fuel level below limit"},

    //BOOLEAN: H13
    {"H13.1",""},
};

void error_description(const char* code, char *buff_desc)
{
    char buff_code[ERRORSYS_STR_CODE_MAX_SIZE];

    uint8_t num_errors = sizeof(errorsys_db)/sizeof(errorsys_db[0]);

    for (uint8_t i=0; i<num_errors; i++)
    {
        strcpy_P(buff_code, (char *) &errorsys_db[i].code );
        if (strcmp(buff_code,code) == 0)
        {
            strcpy_P(buff_desc, (char *) &errorsys_db[i].desc);
            return ;
        }
    }

    strcpy_P(buff_desc, (PGM_P) PSTR("Code not found") );
}

uint8_t error_mgm(long _delay, uint8_t autoerase, const char *code, const char *param0)
{
    int8_t row0=0;
    int8_t row1=1;
    int8_t row2=2;
    int8_t row3=3;

    char buff_desc[ERRORSYS_STR_DESC_MAX_SIZE];
    char str[SYSPRINT_STR_MAX_SIZE];

    if (strcmp_P(code,(PGM_P) PSTR("H10.1"))==0)
    {
        row0= -1;
        sysPrint_lcd_serialPrintln_P(row1,0,PSTR("[ERROR: H10.1]"));
        error_description("H10.1", buff_desc);

        //
        strcpy_P(str, (PGM_P) PSTR("["));
        strcat(str, buff_desc);
        strcat_P(str, (PGM_P) PSTR("]"));
        //
        sysPrint_lcd_serialPrintln(row2,0, str);

        //
        strcpy_P(str, (PGM_P) PSTR("[SD card code:"));
        strcat(str, param0);
        strcat_P(str, (PGM_P) PSTR("]"));
        //

        sysPrint_lcd_serialPrintln(row3,0, str);
    }

    if (strcmp_P(code,(PGM_P) PSTR("H10.2"))==0)
    {
        row0 = row3= -1;
        sysPrint_lcd_serialPrintln_P(row1,0,PSTR("[ERROR:H10.2]"));
        error_description("H10.2", buff_desc);

        //
        strcpy_P(str, (PGM_P) PSTR("["));
        strcat(str, buff_desc);
        strcat_P(str, (PGM_P) PSTR("]"));
        //
        sysPrint_lcd_serialPrintln(row2,0, str);
    }

    if (strcmp_P(code,(PGM_P) PSTR("H10.3"))==0)
    {
        row0 = -1;

        //
        strcpy_P(str, (PGM_P) PSTR("[File:"));
        strcat(str, param0);
        strcat_P(str, (PGM_P) PSTR("]"));
        //
        sysPrint_lcd_serialPrintln(row1,0, str);

        sysPrint_lcd_serialPrintln_P(row2,0,PSTR("[ERROR:H10.3]"));

        error_description("H10.3", buff_desc);

        //
        strcpy_P(str, (PGM_P) PSTR("["));
        strcat(str, buff_desc);
        strcat_P(str, (PGM_P) PSTR("]"));
        //
        sysPrint_lcd_serialPrintln(row3,0, str);
    }

    if (strcmp_P(code,(PGM_P) PSTR("H10.4"))==0)
    {
        row0 = -1;

        //
        strcpy_P(str, (PGM_P) PSTR("[Var:"));
        strcat(str, param0);
        strcat_P(str, (PGM_P) PSTR("]"));
        //
        sysPrint_lcd_serialPrintln(row1,0, str);

        sysPrint_lcd_serialPrintln_P(row2,0,PSTR("[ERROR:H10.4]"));

        error_description("H10.4", buff_desc);
        //
        strcpy_P(str, (PGM_P) PSTR("["));
        strcat(str, buff_desc);
        strcat_P(str, (PGM_P) PSTR("]"));
        //
        sysPrint_lcd_serialPrintln(row3,0, str);
    }

    if (_delay>0)
    {
        delay(_delay);
    }

    if (autoerase == ERROR_LCD_AUTOERASE_ON)
    {
        lcd.clearRows4x20(row0,row1,row2,row3);
    }

    return 1;
}
