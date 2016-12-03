#include <Arduino.h>
#include "vscroll.h"

#define LCD_ROW 4
#define LCD_COL 20

void VScroll::set_db(String * db_data)//Util for local-variable
{
    db.data = db_data;
}

void VScroll::init(String * db_data, int16_t db_dbsize, String * dispShowBuff_data, uint8_t  dispShowBuff_numline,
                   uint8_t mark//feature
                  )
{
    set_db(db_data);
    //    db.data     = db_data;
    db.dbsize   = db_dbsize;


    dispShowBuff.data = dispShowBuff_data ;

    //+++++++++++
    //dispShowBuff.numline =  dispShowBuff_numline;
    if (db_dbsize <= LCD_ROW)//4
    {
        if (dispShowBuff_numline<=db_dbsize)
            dispShowBuff.numline = dispShowBuff_numline;
        else //error-> truncar
            dispShowBuff.numline = db_dbsize;
    }
    else
    {
        if (dispShowBuff_numline <= LCD_ROW)
            dispShowBuff.numline = dispShowBuff_numline;
        else
            dispShowBuff.numline = LCD_ROW;
    }
    //+++++++++++


    db.base     = 0x0000;
    db.offset   = 0x0000;

    feature.bf.mark = mark;//on off mark "<"

    dispShowBuff_filldata();

    //if (feature.bf.mark)
    //    add_mark( &dispShowBuff.data[db.offset] );//at Offset actual
}

void VScroll::feat_mark_ON(void)
{
    feature.bf.mark = 1;//on off mark "<"
}

void VScroll::feat_mark_OFF(void)
{
    feature.bf.mark = 0;//on off mark "<"
}

void VScroll::add_mark(String * pstr)
{
    char arr[LCD_COL + 1];
    pstr->toCharArray(arr, LCD_COL + 1);
    arr[LCD_COL - 1] = '<';
    *pstr = arr;

    //    lcd.setCursor(LCD_COL-1, db.offset);
    //    lcd.write(0x7F);

}

void VScroll::del_mark(String * pstr)
{
    char arr[LCD_COL + 1];
    pstr->toCharArray(arr, LCD_COL + 1);
    arr[LCD_COL - 1] = ' ';
    *pstr = arr;
}
//void VScroll::del_mark_atPosc(void)
//{
//    del_mark(&dispShowBuff.data[db.offset+1]);
//     for (uint8_t i=0; i<dispShowBuff.numline; i++)
//        dispShowBuff.data[i] = db.data[db.base+i];
//}

void VScroll::dispShowBuff_filldata(void)
{
    uint8_t i;
    for (i = 0; i < dispShowBuff.numline; i++)
        dispShowBuff.data[i] = db.data[db.base + i];

    if (feature.bf.mark)
    {
        add_mark( &dispShowBuff.data[db.offset] );
    }
}

void VScroll::process_signal(void)
{
    int16_t base_last;
    int16_t limit_sup = dispShowBuff.numline - 1;
    //int16_t limit_inf = 0;

    if (signal == SIGN_UP)
    {
        db.offset--;

        if ( db.offset < 0)//limit_inf
        {
            db.offset = 0;//limit_inf

            db.base--;
            if (db.base < 0)
                db.base = 0x00;
        }
        else
        {
            if (feature.bf.mark)
                del_mark(&dispShowBuff.data[db.offset + 1]);
        }
    }
    else
    {
        db.offset++;

        if ( db.offset > limit_sup )
        {
            db.offset = limit_sup ;

            base_last = db.base;
            db.base++;
            if ( ( (db.dbsize - 1) - db.base) < limit_sup)
                db.base = base_last;
        }
        else
        {
            if (feature.bf.mark)
                del_mark(&dispShowBuff.data[db.offset - 1]);
        }
    }
}

uint16_t VScroll::get_markPosc(void)
{
    return (uint16_t)(db.base + db.offset);
}

void VScroll::sign_up(void)
{
    signal = SIGN_UP;
}
void VScroll::sign_down(void)
{
    signal = SIGN_DOWN;
}
void VScroll::clear_sign(void)
{
    signal = SIGN_CLEAR;
}
void VScroll::job(void)
{
    if (signal != SIGN_CLEAR)
    {
        process_signal();
        clear_sign();
    }

    dispShowBuff_filldata();
}
//hay 2 tipos de jobs...statics y dynamics

VScroll vscroll;
