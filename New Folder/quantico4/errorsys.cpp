#include <Arduino.h>
#include "system.h"
#include "errorsys.h"

#define ERRORSYS_CODE_MAXCHAR (6+1)
#define ERRORSYS_DESC_MAXCHAR 45

struct _errorsys_db
{
    const char code[ERRORSYS_CODE_MAXCHAR];
    const char desc[ERRORSYS_DESC_MAXCHAR];
};

const struct _errorsys_db errorsys_db[] PROGMEM=
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


//String error_description(const char* code)
void error_description(const char* code, char *buff_desc)
{
    char buff_code[ERRORSYS_CODE_MAXCHAR];
    //char buff_desc[ERRORSYS_DESC_MAXCHAR];

    uint8_t num_errors = sizeof(errorsys_db)/sizeof(errorsys_db[0]);

    for (uint8_t i=0; i<num_errors; i++)
    {
        strcpy_P(buff_code, (char *) &errorsys_db[i].code );
        if (strcmp(buff_code,code) == 0)
        {
            strcpy_P(buff_desc, (char *) &errorsys_db[i].desc);
            //return (String)buff_desc;
            return ;
        }
    }

    strcpy(buff_desc,"Code not found");
    //return (String)"Code not found";
}

uint8_t error_mgm(long _delay, uint8_t autoerase, const char *code, String param0)
{
    int8_t row0=0;
    int8_t row1=1;
    int8_t row2=2;
    int8_t row3=3;

    char buff_desc[ERRORSYS_DESC_MAXCHAR];

    if (strcmp(code,"H10.1")==0)
    {
        row0= -1;
        sysPrint_lcd_serialPrintln(row1,0,"[ERROR: H10.1]");
        error_description("H10.1", buff_desc);
        sysPrint_lcd_serialPrintln(row2,0,"[" +String(buff_desc)+"]");
        sysPrint_lcd_serialPrintln(row3,0,"[SD card code:"+ param0 +"]");
    }

    if (strcmp(code,"H10.2")==0)
    {
        row0 = row3= -1;
        sysPrint_lcd_serialPrintln(row1,0,"[ERROR:H10.2]");
        error_description("H10.2", buff_desc);
        sysPrint_lcd_serialPrintln(row2,0,"["+String(buff_desc)+"]");
    }

    if (strcmp(code,"H10.3")==0)
    {
        row0 = -1;
        sysPrint_lcd_serialPrintln(row1,0,"[File:"+ param0 +"]");
        sysPrint_lcd_serialPrintln(row2,0,"[ERROR:H10.3]");
        error_description("H10.3", buff_desc);
        sysPrint_lcd_serialPrintln(row3,0,"["+String(buff_desc)+"]");
    }

    if (strcmp(code,"H10.4")==0)
    {
        row0 = -1;
        sysPrint_lcd_serialPrintln(row1,0,"[Var:"+ param0 +"]");
        sysPrint_lcd_serialPrintln(row2,0,"[ERROR:H10.4]");
        error_description("H10.4", buff_desc);
        sysPrint_lcd_serialPrintln(row3,0,"["+String(buff_desc)+"]");
    }

    if (_delay>0)
        delay(_delay);

    if (autoerase == ERROR_LCD_AUTOERASE_ON)
        lcd.clearRows4x20(row0,row1,row2,row3);
}
