#include <Arduino.h>
#include <LiquidCrystal.h>
#include "lcd_fxaux.h"
#include "system.h"

#include "vscroll.h"

void mylcd::printAtPosc(uint8_t fil, uint8_t col, const char *str)
{
    setCursor(col, fil);
    print(String(str));
}

void mylcd::printAtPosc_char(uint8_t fil, uint8_t col, const char c)
{
    setCursor(col, fil);
    write(c);
}
//void mylcd::printTrimmReplaceByDot(uint8_t fil, uint8_t col, char *str)//todos los arrays/buffers q imprimen en LCD DEBEN DE SER >= LCD_COL+1
//void mylcd::printTrimmReplaceByDot(char *str)//todos los arrays/buffers q imprimen en LCD DEBEN DE SER >= LCD_COL+1
//{
////    if (strlen(str) > LCD_COL)
////    {
//        str[LCD_COL-4] = '.';
//        str[LCD_COL-3] = '.';
//        str[LCD_COL-2] = '.';
//        str[LCD_COL-1] = '\0';
//
////        printAtPosc(fil, col, str);
////    }
////    else
////        printAtPosc(fil, col, str);
//
//}

void mylcd::clearRows4x20(int8_t row0, int8_t row1, int8_t row2, int8_t row3)
{
    if (row0 > -1)
    {
        setCursor(0, row0);
        print("                    ");
    }
    if (row1 > -1)
    {
        setCursor(0, row1);
        print("                    ");
    }
    if (row2 > -1)
    {
        setCursor(0, row2);
        print("                    ");
    }
    if (row3 > -1)
    {
        setCursor(0, row3);
        print("                    ");
    }
}


int8_t lcdan_str_get_center_hor(const char *str)
{
    int8_t pos = 0;
    uint8_t length = strlen(str);

    if (length < LCD_COL)
        pos = (LCD_COL - length) >> 1; //2

    return pos;
}
uint8_t lcdan_str_get_align_col(const char *str_src, int8_t col_align)
{
    if (col_align == LCDAN_STR_FORMAT_ALIGN_CENTER)
    {
        return  lcdan_str_get_center_hor(str_src);
    }
    //else if ()//LEFT RIGHT
}
///////////////////////// unificar con sysPrint ////////////////////////////////
//special to print VSCROLL over LCD 20X4

//el programador sera responsable de:
//if (pos_i= 0) && (str_src >COL)
//--> no align, untouch str_src

//ojo trunca al tamano de columnas imprimibles en LCD alfanu..
//CONDITION: strlen(str_dest) >= LCDAN_STR_MAX_SIZE
////////////////////////////////////////////////////////////////////////////////
void lcdan_str_lineformat_align(char *str_dest,  const char *str_src, int8_t pos_i /* = 0 */)
{
    int8_t i, pos_f, x;

    str_dest[LCDAN_STR_MAX_SIZE-1] = '\0';         //[''   ...''0] posc[20]='\0'
    for (i = LCDAN_STR_MAX_SIZE-2; i >= 0; i--)    //blank all elements
        str_dest[i] = ' ';

    if (pos_i < 0)
        pos_i = lcdan_str_get_align_col(str_src, LCDAN_STR_FORMAT_ALIGN_CENTER);

    pos_f = pos_i + strlen(str_src);

    if (pos_f > LCD_COL)
        pos_f = LCD_COL;//trunc to max pos= LCD_COL

    x = 0;
    for (i = pos_i; i < pos_f; i++, x++)
        str_dest[i] = str_src[x];
}

//////////////////////////////////////////////////////////
void lcdan_str_lineformat_align_P(char *str_dest,  const char *str_src, int8_t pos_i /* = 0 */)//default uint8_t posc_i=0
{
    char _str_src[LCDAN_STR_MAX_SIZE];
    strncpy_P(_str_src,  (const char *)str_src, LCDAN_STR_MAX_SIZE);
    _str_src[LCDAN_STR_MAX_SIZE-1] = '\0';

    lcdan_str_lineformat_align(str_dest, (const char *) _str_src, pos_i);//default uint8_t posc_i=0
}
//////////////////////////////////////////////////////////
//3dot at end of line
//todos los arrays/buffers q imprimen en LCD DEBEN DE SER >= LCD_COL+1
void lcdan_str_lineformat_trimEOL3dots(char *str)
{
    str[LCD_COL-3] = '.';
    str[LCD_COL-2] = '.';
    str[LCD_COL-1] = '.';
    str[LCD_COL] = '\0';
}
