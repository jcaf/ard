#include <Arduino.h>
#include "system.h"

#include "menu_stackInDetail_V.h"

#include "vscroll.h"
#include "lcd_fxaux.h"
#include "menu_bkground.h"
#include "ikey.h"
#include "kb.h"
#include "LTC68042.h"
#include "ltc6804.h"
#include "stackError.h"
#include "getString.h"
#include "menu.h"
#include "menu_mainMenu.h"

static void mBkground_stackInDetail_V_fxKey0(void)
{
    vscroll.stack_pop();
    menu_setMenu(MGO_MAIN_MENU);//mandar todos el SP a 0 es mas seguro
}
static void mBkground_stackInDetail_V_fxKey1(void)
{
    vscroll.sign_up();
}
static void mBkground_stackInDetail_V_fxKey2(void)
{
    vscroll.sign_down();
}
static void mBkground_stackInDetail_V_fxKey3(void)
{
}
static void mBkground_stackInDetail_V_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setSubMenu(MGO_MBKGROUND_INSIDESTACKSUMMARY_JOB);
}

PTRFX_retVOID const mBkground_stackInDetail_V_fxKey[KB_NUM_KEYS] PROGMEM=
{mBkground_stackInDetail_V_fxKey0, mBkground_stackInDetail_V_fxKey1, mBkground_stackInDetail_V_fxKey2, mBkground_stackInDetail_V_fxKey3, mBkground_stackInDetail_V_fxKey4};

static void mBkground_stackInDetail_V_updateDbData(String *db);

void mBkground_stackInDetail_V_job(void)
{
    uint8_t num_ic = BOARD_get_num_ic_currentboard();
    uint8_t db_size;

    String db_mBkground_stackInDetail_V[LTC_NUM_CELLS * 2]; //24 cell

    //mBkground_stackInDetail_V_updateDbData(db_mBkground_stackInDetail_V);

    if(thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mBkground_stackInDetail_V_fxKey);

        db_size = get_db_size();

        vscroll.init(db_mBkground_stackInDetail_V, db_size, dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        thismenu_sm0 = -1;
    }

    if(!ltc6804.app.synch.data_invalid)
        mBkground_stackInDetail_V_updateDbData(db_mBkground_stackInDetail_V);

    if(1) //cualquier cambio en board, se regresa al menu Summary
    {
        vscroll.set_db(db_mBkground_stackInDetail_V);
        vscroll.job();
        lcd_show_disp();

//        if(!ltc6804.app.synch.data_invalid)
//            mBkground_stackInDetail_V_updateDbData(db_mBkground_stackInDetail_V);
    }
}

static void mBkground_stackInDetail_V_updateDbData(String *db)
{
    //String str;
    uint8_t num_ic, num_cell, NUM_CELL_TOP;
    uint8_t cv, stack_LU;

    float CV;

    if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
    {
        if(select_2_12V[BOARD_get_num_ic_currentboard()] == 2)
            NUM_CELL_TOP = NUM_CELL_2V_MAX;
        else
            NUM_CELL_TOP = NUM_CELL_12V_MAX ;

        for(num_cell = 0; num_cell < NUM_CELL_TOP; num_cell++)
        {
            CV = vcellF[BOARD_get_num_ic_currentboard()][num_cell];
            db[num_cell] = str_clearPrint(getString_cell_voltage(CV, num_cell + 1), 4);
        }
    }
    else if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
    {
        //C1...CX - CX+1...CX+n
         cv=0;
        for (uint8_t stack_LU=StackLower; stack_LU<=StackUpper; stack_LU++)
        {
            num_ic = board[num_board].stack[stack_LU].address;

            if(select_2_12V[num_ic] == 2)
                NUM_CELL_TOP = NUM_CELL_2V_MAX;
            else
                NUM_CELL_TOP = NUM_CELL_12V_MAX ;

            for (num_cell=0; num_cell<NUM_CELL_TOP; num_cell++)
            {
                CV = vcellF[num_ic][num_cell];
                db[cv] = str_clearPrint(getString_cell_voltage(CV, cv + 1), 4);
                cv++;
            }
        }

    }
}



