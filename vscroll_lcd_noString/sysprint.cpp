#include <Arduino.h>
#include "sysprint.h"

#include "system.h"

void sysPrint_serialPrint(const char *str)
{
    if (mysystem.funct_mode == FUNCT_MODE_DEBUG)
        Serial.print(str);
}

void sysPrint_serialPrint_P(const char *str)
{
    char _str[SYSPRINT_STR_MAX_SIZE];
    strncpy_P(_str, (const char *) str, SYSPRINT_STR_MAX_SIZE);
    _str[SYSPRINT_STR_MAX_SIZE-1] = '\0';

    sysPrint_serialPrint(_str);
}

void sysPrint_serialPrintln(const char *str)
{
    if (mysystem.funct_mode == FUNCT_MODE_DEBUG)
        Serial.println(str);
}

void sysPrint_serialPrintln_P(const char *str)
{
    char _str[SYSPRINT_STR_MAX_SIZE];
    strncpy_P(_str, (const char *) str, SYSPRINT_STR_MAX_SIZE);
    _str[SYSPRINT_STR_MAX_SIZE-1] = '\0';

    sysPrint_serialPrintln(_str);
}

/////////////////////////////////////////////////////////////////////////////////////////
void sysPrintLCDan_XY(uint8_t fil, int8_t col, const char *str)
{
    lcd.printAtPosc(fil,col, str);
}

void sysPrintLCDan_XY_wlineClr(uint8_t fil, int8_t col, const char *str_src)
{
    char str_dest[LCDAN_STR_MAX_SIZE];//str_dest[SYSPRINT_STR_MAX_SIZE];
    lcdan_str_lineformat_align(str_dest, str_src, col);
    sysPrintLCDan_XY(fil, 0, str_dest);
}
void sysPrintLCDan_XY_wlineClr_wEOL3dots(uint8_t fil, int8_t col, const char *str_src)
{
    char str_dest[LCDAN_STR_MAX_SIZE];
    lcdan_str_lineformat_align(str_dest, str_src, col);

    if (col < (LCD_COL-3))//hasta la col = 16
    {
        if (col + strlen(str_src) > LCD_COL)
        {
            lcdan_str_lineformat_trimEOL3dots(str_dest);
        }
    }

    sysPrintLCDan_XY(fil, 0, str_dest);
}

void sysPrintLCDan_XY_wlineUnclr(int8_t fil, int8_t col_i, const char *str_src)
{
    int8_t i,col_f;

    if (col_i < 0)
        col_i = lcdan_str_get_align_col(str_src, LCDAN_STR_FORMAT_ALIGN_CENTER);

    col_f = col_i + strlen(str_src);

    if (col_f > LCD_COL)
        col_f = LCD_COL;//trunc to max pos= LCD_COL
    //
    lcd.setCursor(col_i, fil);
    uint8_t x=0;
    for (i = col_i; i < col_f; i++, x++)
        lcd.write(str_src[x]);
    //
}
void sysPrintLCDan_XY_wlineUnclr_wEOL3dots(int8_t fil, int8_t col_i, const char *str_src)
{
  int8_t i,col_f;

    if (col_i < 0)
        col_i = lcdan_str_get_align_col(str_src, LCDAN_STR_FORMAT_ALIGN_CENTER);

      size_t length = strlen(str_src);
      col_f = col_i + length;
      if (col_f > LCD_COL)
        col_f = LCD_COL;//trunc to max pos= LCD_COL
      
      if (col_i < (LCD_COL-3))//hasta la col = 16
      {
          if (col_i + length > LCD_COL)
          {
              lcd.setCursor(LCD_COL-3, fil);
              for (i = 0; i < 3; i++)
                lcd.write('.');//
              
              //
              col_f = LCD_COL-3;
          }
      }

    //
    lcd.setCursor(col_i, fil);
    uint8_t x=0;
    for (i = col_i; i < col_f; i++, x++)
    {
      lcd.write(str_src[x]);
    }
    //

}
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////
void sysPrintLCDan_XY_P(uint8_t fil, int8_t col, const char *str)
{
    //
    char _str[LCDAN_STR_MAX_SIZE];
    strncpy_P(_str,  (const char *)str, LCDAN_STR_MAX_SIZE);
    _str[LCDAN_STR_MAX_SIZE-1] = '\0';
    //
    sysPrintLCDan_XY(fil, col, _str);
}

void sysPrintLCDan_XY_wlineClr_P(uint8_t fil, int8_t col, const char *str_src)
{
    char str_dest[LCDAN_STR_MAX_SIZE];//str_dest[SYSPRINT_STR_MAX_SIZE];
    lcdan_str_lineformat_align_P(str_dest, str_src, col);
    sysPrintLCDan_XY(fil, 0, str_dest);
}
void sysPrintLCDan_XY_wlineClr_wEOL3dots_P(uint8_t fil, int8_t col, const char *str_src)
{
    //
    char _str[LCDAN_STR_MAX_SIZE];
    strncpy_P(_str,  (const char *)str_src, LCDAN_STR_MAX_SIZE);
    _str[LCDAN_STR_MAX_SIZE-1] = '\0';
    //
    sysPrintLCDan_XY_wlineClr_wEOL3dots(fil, col, _str);
}

void sysPrintLCDan_XY_wlineUnclr_P(int8_t fil, int8_t col_i, const char *str_src)
{
    //
    char _str[LCDAN_STR_MAX_SIZE];
    strncpy_P(_str,  (const char *)str_src, LCDAN_STR_MAX_SIZE);
    _str[LCDAN_STR_MAX_SIZE-1] = '\0';
    //
    sysPrintLCDan_XY_wlineUnclr(fil, col_i, _str);
}
void sysPrintLCDan_XY_wlineUnclr_wEOL3dots_P(int8_t fil, int8_t col_i, const char *str_src)
{
    //
    char _str[LCDAN_STR_MAX_SIZE];
    strncpy_P(_str,  (const char *)str_src, LCDAN_STR_MAX_SIZE);
    _str[LCDAN_STR_MAX_SIZE-1] = '\0';
    //
    sysPrintLCDan_XY_wlineUnclr_wEOL3dots(fil, col_i, _str);
}
void sysPrint_lcd_serialPrintln(uint8_t fil, int8_t col, char *str)
{
    sysPrintLCDan_XY_wlineClr_wEOL3dots(fil,col, str);
    sysPrint_serialPrintln(str);
}

void sysPrint_lcd_serialPrintln_P(uint8_t fil, int8_t col, const char *str)
{
    sysPrintLCDan_XY_wlineClr_wEOL3dots_P(fil,col, str);
    sysPrint_serialPrintln_P(str);
}
