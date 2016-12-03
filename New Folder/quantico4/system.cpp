#include <Arduino.h>
#include "system.h"

struct _system system;

void sysPrint_serialPrint(String msg)
{
    if (system.funct_mode == FUNCT_MODE_DEBUG)
    {
        Serial.print(msg);
    }
}

void sysPrint_serialPrintln(String msg)
{
    if (system.funct_mode == FUNCT_MODE_DEBUG)
    {
        Serial.println(msg);
    }
}

/////////////////////////////////////////////////
/// \brief
///
/// \param
/// \param
/// \return
///
/////////////////////////////////////////////////
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
    char msg[25];
    strcpy_P(msg, (const char *) pmsg);

    int8_t col_posc = col;

    if (col_posc< 0)
    {
        if (col == STR_CENTER)
            col_posc = str_center(String(msg));
    }

    lcd.printTrimmReplaceByDot(fil, 0, str_clearPrint(String(msg), col_posc));
}

/////////////////////////////////////////////////
/// \brief
///
/// \param
/// \param
/// \return
///
/////////////////////////////////////////////////

void sysPrint_lcd_serialPrintln(uint8_t fil, int8_t col, String msg)
{
    sysPrint_lcdPrint(fil,col, msg);
    sysPrint_serialPrintln(msg);
}


/********************************************************************************
uint8_t MAC[6];
    if (MAC_String2HexArray( mainvar_getvalue("MAC") ,MAC) )
    {
        for (int i=0; i<6; i++)
            Serial.println(MAC[i]);
    }
********************************************************************************/
//uint8_t MAC_String2HexArray(String setting_value, uint8_t *MAC)
//{
//#define arr_leng 40
//#define buff_length 10
//#define _buff_clear() do{for (size_t i=0;i<buff_length;i++){buff[i]=0;} }while(0)
//
//    uint8_t parseok=0;
//    char arr[arr_leng];
//    char buff[buff_length];
//    uint8_t ca,cb,cm;
//    //uint8_t MAC[6];
//    ca=cb=cm=0x00;
//
//    setting_value.toCharArray(arr,setting_value.length()+1);
//    _buff_clear();
//
//    while (arr[ca] != '\0')
//    {
//        if (arr[ca]==',')
//        {
//            MAC[cm++] =  (uint8_t) strtoul(buff, NULL, 16);
//            ca++;//1++ for skip comma
//            _buff_clear();
//            cb=0;
//        }
//        buff[cb++]=arr[ca++];
//    }
//    MAC[5] = (uint8_t) strtoul(buff, NULL, 16);
//
//    parseok = 1;
//
//    return parseok;
////    for (int i=0; i<6; i++) Serial.println(MAC[i]);
//}
