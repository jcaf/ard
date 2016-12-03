#include <Arduino.h>
#include "system.h"
#include "LTC68042.h"

#define SYSPRINT_SERIALPRINT_MSG_MAX_LENGTH 50

struct _system system;

void sysPrint_serialPrint(String msg)
{
    if (system.funct_mode == FUNCT_MODE_DEBUG)
    {
        Serial.print(msg);
    }
}

void sysPrint_serialPrintPSTR( const char *pmsg)
{
    char msg[SYSPRINT_SERIALPRINT_MSG_MAX_LENGTH];
    strcpy_P(msg, (const char *) pmsg);

    if (system.funct_mode == FUNCT_MODE_DEBUG)
    {
        Serial.print(String(msg));
    }
}

void sysPrint_serialPrintln(String msg)
{
    if (system.funct_mode == FUNCT_MODE_DEBUG)
    {
        Serial.println(msg);
    }
}

void sysPrint_serialPrintlnPSTR( const char *pmsg)
{
    char msg[SYSPRINT_SERIALPRINT_MSG_MAX_LENGTH];
    strcpy_P(msg, (const char *) pmsg);

    if (system.funct_mode == FUNCT_MODE_DEBUG)
    {
        Serial.println(String(msg));
    }
}


void sysPrint_lcdPrint(uint8_t fil, int8_t col, String msg)
{
    int8_t col_posc = col;

    if (col_posc< 0)
    {
        if (col == STR_CENTER)
            col_posc = str_center(msg);
    }

    lcd.printTrimmReplaceByDot(fil, 0, str_clearPrint(msg, col_posc));
}

void sysPrint_lcdPrintPSTR(uint8_t fil, int8_t col, const char *pmsg)
{
    char msg[SYSPRINT_SERIALPRINT_MSG_MAX_LENGTH];
    strcpy_P(msg, (const char *) pmsg);

    int8_t col_posc = col;

    if (col_posc< 0)
    {
        if (col == STR_CENTER)
            col_posc = str_center(String(msg));
    }

    lcd.printTrimmReplaceByDot(fil, 0, str_clearPrint(String(msg), col_posc));
}

void sysPrint_lcd_serialPrintln(uint8_t fil, int8_t col, String msg)
{
    sysPrint_lcdPrint(fil,col, msg);
    sysPrint_serialPrintln(msg);
}

void sysPrint_lcd_serialPrintlnPSTR(uint8_t fil, int8_t col,  const char *pmsg)
{
    sysPrint_lcdPrintPSTR(fil,col, pmsg);
    sysPrint_serialPrintlnPSTR(pmsg);
}

void SPI_deselectAllperipherals(void)
{
    digitalWrite(WIZNET_CS, HIGH);
    pinMode(WIZNET_CS, OUTPUT);

    digitalWrite(SDCARD_CS, HIGH);
    pinMode(SDCARD_CS, OUTPUT);

    digitalWrite(LTC6804_CS, HIGH);
    pinMode(LTC6804_CS, OUTPUT);
}

