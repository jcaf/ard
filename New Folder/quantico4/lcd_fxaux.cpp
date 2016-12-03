#include <Arduino.h>
#include <LiquidCrystal.h>
#include "lcd_fxaux.h"
#include "system.h"

void mylcd::printAtPosc(uint8_t fil, uint8_t col, String s)
{
    setCursor(col,fil);
    print(s);

}


//void lcd_print_at_posc(uint8_t fil, uint8_t col, String s)
//{
//    lcd.setCursor(col,fil);
//    lcd.print(s);
//}

void mylcd::printTrimmReplaceByDot(uint8_t fil, uint8_t col, String s)
{

    char buff[LCD_COL+1];

    if (s.length()>LCD_COL)
    {
        s.toCharArray(buff,18);//0..16
        buff[17]='.';
        buff[18]='.';
        buff[19]='.';
        buff[20]='\0';

        s = String(buff);
    }
    printAtPosc(fil,col,s);
}


void mylcd::clearRows4x20(int8_t row0, int8_t row1, int8_t row2, int8_t row3)
{
    if (row0 > -1)
    {
        setCursor(0,row0);
        print("                    ");
    }
    if (row1 > -1)
    {
        setCursor(0,row1);
        print("                    ");
    }
    if (row2 > -1)
    {
        setCursor(0,row2);
        print("                    ");
    }
    if (row3 >-1)
    {
        setCursor(0,row3);
        print("                    ");
    }
}
//void lcd_print_trimmReplacebydot(uint8_t fil, uint8_t col, String s)
//{
//    char buff[LCD_COL+1];
//    if (s.length()>LCD_COL)
//    {
//        s.toCharArray(buff,18);//0..16
//        buff[17]='.';
//        buff[18]='.';
//        buff[19]='.';
//        buff[20]='\0';
//            s = String(buff);
//    }
//    lcd_print_at_posc(fil,col,s);
//}

uint8_t str_center(String str)
{
    uint8_t posc=0;
    uint8_t length = str.length();

    if (length <LCD_COL)
    {
        posc = (LCD_COL - length)>>1; //2
//        if (posc>0)
//            posc--;
    }
    return posc;
}


//void lcd_show_disp(void)
void lcd_show_disp(uint8_t fil_ini, uint8_t fil_end)
{
    if (fil_end == 0)
    {
        fil_end = LCD_ROW_3;//x defect
    }
    else
    {
        if (fil_end > LCD_ROW_3)
            fil_end = LCD_ROW_3;
    }

    uint8_t cr = 0; //count row
    for (uint8_t r = fil_ini; r <= fil_end; r++)
    {
        //lcd_print_at_posc(r,0,dispShowBuff[cr]);
        lcd.setCursor(0, r);
        lcd.print(dispShowBuff[cr]);
        cr++;
    }
}


String str_clearPrint(String str,  uint8_t posc_i)//default uint8_t posc_i=0
{
    char arr[LCD_COL + 1];
    char arr1[LCD_COL + 1];
    uint8_t length;
    uint8_t c = 0;

    if (str.length()<= LCD_COL)
    {

        for (uint8_t i = 0; i < LCD_COL; i++)
        {
            arr[i] = ' ';
        }

        arr[LCD_COL] = '\0';

        length = str.length();
        str.toCharArray(arr1, length + 1);

        uint8_t posc_f = posc_i + length;

        if (posc_f > LCD_COL)
            posc_f = LCD_COL;

        for (uint8_t i = posc_i; i<posc_f; i++)
        {
            arr[i] = arr1[c];
            c++;
        }
        return String(arr);
    }
    else
        return str;

}
