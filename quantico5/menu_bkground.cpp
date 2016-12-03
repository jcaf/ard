#include <Arduino.h>
#include "menu.h"
#include "menu_bkground.h"
#include "system.h"
#include "ikey.h"
#include "kb.h"
#include "vscroll.h"
#include "lcd_fxaux.h"
#include "LTC68042.h"
#include "ltc6804.h"
#include "stackError.h"
#include "getString.h"

#include "menu.h"
#include "menu_stackInDetail_V.h"
#include "menu_stackInDetail_E.h"
#include "menu_mainMenu.h"

void board_event(void)
{
    	if (boardD.set_text)//by software (show again on-demand)
		{
			boardD.set_text = 0;
			board_setText();
		}
		if(boardD.AchangeInBoard)//by hardware
		{
			board_setText();

			int8_t current_menu = menu_getSubMenu();
			if (
				(current_menu == MGO_MBKGROUND_STACKINDETAIL_V_JOB ) ||
				(current_menu == MGO_MBKGROUND_STACKINDETAIL_E_JOB)
				)
			{
				vscroll.stack_pop();//1 pop x default
			}
			if (
				(current_menu == MGO_MBKGROUND_STACKINDETAIL_E_UNDERVOLT_JOB) ||
				(current_menu == MGO_MBKGROUND_STACKINDETAIL_E_OVERVOLT_JOB) ||
				(current_menu == MGO_MBKGROUND_STACKINDETAIL_E_OPENWIRE_JOB)
			   )
			{
				vscroll.stack_pop(2);//2 pop
			}
			menu_setSubMenu(MGO_MBKGROUND_SHOWSTACKSUMMARY_JOB);
		}

}

void board_setText(void)
{
    String title;

    if(num_board == 0)
        title = "[MASTER]";
    else
        title = "[EXT. " + String(num_board) + "]";

    if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
    {
        //title +="[STACK1+2]"
        title += "[STACK" + String(board[num_board].stack[StackLower].address + 1) + "+" + String(board[num_board].stack[StackUpper].address + 1) + "]";

        board[num_board].stacks.num_stack = 0;
    }
    else  if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
    {
        //S = # de stack
        uint8_t opt = board[num_board].stacks.num_stack;

        if(board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire)
            //title +="[STACK S]";
            title += "[STACK " + String(board[num_board].stack[opt].address + 1) + "]";
        else if(board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist)
            //title +="[STACK S+(S+1)] S";
            title += "[STACK" + String(board[num_board].stack[StackLower].address + 1) + "+" + String(board[num_board].stack[StackUpper].address + 1) + "] " + String(board[num_board].stack[opt].address + 1);

        if(ltc_connected[ board[num_board].stack[opt].address] == LTC_DISCONNECTED)
        {
            lcd.setCursor(0, 1);
            lcd.print(str_clearPrint("", 0));
            String txt = "STACK " + String(board[num_board].stack[opt].address + 1);
            lcd.setCursor(0, 2);
            lcd.print(str_clearPrint(txt, 6));
            lcd.setCursor(0, 3);
            lcd.print(str_clearPrint("NO CONECTADO", 4));
        }
    }

    title = str_clearPrint(title, 0);
    lcd.setCursor(0, 0);
    lcd.print(title);

     //+-add
    //if (thismenu_sm0 == STACKSUMMARY)
    if (menu_getSubMenu() == MGO_MBKGROUND_SHOWSTACKSUMMARY_JOB)
        {lcd.setCursor(18,0);lcd.print("<>");}
    //-+
}

static void mBkground_showStackSummary_fxKey0(void)//Menu
{
    menu_setMenu(MGO_MAIN_MENU);
    //menu_setSubMenu(-1);
}
static void mBkground_showStackSummary_fxKey1(void)//Up
{
    if(--board[num_board].stacks.num_stack < 0)  //board[num_board].stack.option.limit_inf )
    {
        board[num_board].stacks.num_stack = 0;

        if(--num_board < 0)
        {
            num_board  = NUM_BOARD - 1;

            for(uint8_t nb = 0; nb < NUM_BOARD; nb++)
                board[nb].stacks.num_stack = board[num_board].stacks.num_max_stacks - 1;
        }

        board[num_board].unionMode_last = -1; //provoca un cambio para q se reevalue el modo de stack
    }

    boardD.AchangeInBoard = 1;
}
static void mBkground_showStackSummary_fxKey2(void)//Down
{
    if(++board[num_board].stacks.num_stack > (board[num_board].stacks.num_max_stacks - 1))
    {
        board[num_board].stacks.num_stack = (board[num_board].stacks.num_max_stacks - 1);

        if(++num_board > NUM_BOARD - 1)
        {
            num_board  = 0;

            for(uint8_t nb = 0; nb < NUM_BOARD; nb++)
                board[nb].stacks.num_stack = 0;
        }

        board[num_board].unionMode_last = -1; //provoca un cambio para q se reevalue el modo de stack
    }

    boardD.AchangeInBoard = 1;
}
static void mBkground_showStackSummary_fxKey3(void)//Enter
{
    uint8_t opt = board[num_board].stacks.num_stack;

    if(ltc_connected[ board[num_board].stack[opt].address] == LTC_CONNECTED)
    {
        menu_setSubMenu(MGO_MBKGROUND_INSIDESTACKSUMMARY_JOB);

        lcd.setCursor(18,0);lcd.print("  ");//clear "<>"
    }
}
static void mBkground_showStackSummary_fxKey4(void)//eScape
{
}

PTRFX_retVOID const mBkground_showStackSummary_fxKey[KB_NUM_KEYS] PROGMEM=
{mBkground_showStackSummary_fxKey0, mBkground_showStackSummary_fxKey1, mBkground_showStackSummary_fxKey2, mBkground_showStackSummary_fxKey3, mBkground_showStackSummary_fxKey4};

static void mBkground_showStackSummary_updateDbData(String *db_stackSummary);

#define STACK_SUMMARY_NUM_ELEM 4

void mBkground_showStackSummary_job(void)
{
    static uint8_t doThisJob;

    String db_stackSummary[STACK_SUMMARY_NUM_ELEM];
    mBkground_showStackSummary_updateDbData(db_stackSummary);

    if (thismenu_sm0 == 0) //init
    {
        kb_change_keyDo_pgm(mBkground_showStackSummary_fxKey);

        thismenu_sm0 = -1;
    }

    if(boardD.AchangeInBoard)
    {
        if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
        {
            vscroll.init(db_stackSummary, STACK_SUMMARY_NUM_ELEM, dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_OFF);
            doThisJob = 1;
        }
        else  if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
        {
            //S = # de stack
            uint8_t opt = board[num_board].stacks.num_stack;

            if(ltc_connected[ board[num_board].stack[opt].address] == LTC_CONNECTED)
            {
                vscroll.init(db_stackSummary, STACK_SUMMARY_NUM_ELEM, dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_OFF);
                doThisJob = 1;
            }
            else
                doThisJob = 0;
        }
    }

    if(doThisJob)
    {
        vscroll.set_db(db_stackSummary);
        vscroll.job();
        lcd_show_disp();

        if(!ltc6804.app.synch.data_invalid)
            mBkground_showStackSummary_updateDbData(db_stackSummary);
    }
}

static void mBkground_showStackSummary_updateDbData(String *db_stackSummary)
{
    uint8_t tempError_Lower, tempError_Upper;
    uint8_t num_ic = BOARD_get_num_ic_currentboard();

    if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
    {
        //Voltage Stack
        db_stackSummary[0] = str_clearPrint(getString_voltage(stack_voltage_ifcF[num_ic]), 4);
        //Current Stack
        db_stackSummary[1] = str_clearPrint(getString_current(stack_currentF[num_ic]), 4);
        //Temperature Stack
        db_stackSummary[2] = str_clearPrint(getString_temperature(stack_temperatureF[num_ic]), 4);
        //Errors = #
        db_stackSummary[3] = str_clearPrint(getString_error("ERROR= ", get_stackError_total(num_ic)), 4);
    }
    else if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
    {
        //Voltage Stack 1+2
        float CV = stack_voltage_ifcF[ board[num_board].stack[StackLower].address ] + stack_voltage_ifcF[ board[num_board].stack[StackUpper].address];
        db_stackSummary[0] = str_clearPrint(getString_voltage(CV), 4);
        //Current Stack LOWER
        db_stackSummary[1] = str_clearPrint(getString_current(stack_currentF[board[num_board].stack[StackLower].address]), 4);
        //Temperature Stack LOWER
        db_stackSummary[2] = str_clearPrint(getString_temperature(stack_temperatureF[board[num_board].stack[StackLower].address]), 4);

        //Errors = #
        tempError_Lower = get_stackError_total(board[num_board].stack[StackLower].address);
        tempError_Upper = get_stackError_total(board[num_board].stack[StackUpper].address);

        db_stackSummary[3] = str_clearPrint(getString_error("ERROR= ", tempError_Lower+tempError_Upper), 4);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void mBkground_insideStackSummary_fxKey0(void)
{

}
static void mBkground_insideStackSummary_fxKey1(void)
{
    vscroll.sign_up();
}
static void mBkground_insideStackSummary_fxKey2(void)
{
    vscroll.sign_down();
}
static void mBkground_insideStackSummary_fxKey3(void)
{
    vscroll.stack_push();
    vscroll.base_offset_reset();

    switch (vscroll.get_markPosc())
    {
        case 0://V
                menu_setSubMenu(MGO_MBKGROUND_STACKINDETAIL_V_JOB);
            break;

        case 1://I
            break;

        case 2://T
            break;

        case 3://E
            menu_setSubMenu(MGO_MBKGROUND_STACKINDETAIL_E_JOB);
            break;
    }
}
void mBkground_insideStackSummary_fxKey4(void)
{
    menu_setSubMenu(MGO_MBKGROUND_SHOWSTACKSUMMARY_JOB);

    vscroll.feat_mark_OFF();

    //+-add
    //if (thismenu_sm0 == STACKSUMMARY)
        {lcd.setCursor(18,0);lcd.print("<>");}
    //-+
}

PTRFX_retVOID const mBkground_insideStackSummary_fxKey[KB_NUM_KEYS] PROGMEM=
{mBkground_insideStackSummary_fxKey0, mBkground_insideStackSummary_fxKey1, mBkground_insideStackSummary_fxKey2, mBkground_insideStackSummary_fxKey3, mBkground_insideStackSummary_fxKey4};

void mBkground_insideStackSummary_job(void)
{
    String db_stackSummary[STACK_SUMMARY_NUM_ELEM];

    if(thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mBkground_insideStackSummary_fxKey);

        vscroll.init(db_stackSummary, STACK_SUMMARY_NUM_ELEM, dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON, vscroll.base, vscroll.offset);
        //vscroll.feat_mark_ON();

        thismenu_sm0 = -1;
    }

    if(!ltc6804.app.synch.data_invalid)
        mBkground_showStackSummary_updateDbData(db_stackSummary);

    vscroll.set_db(db_stackSummary);
    vscroll.job();
    lcd_show_disp();

//    if(!ltc6804.app.synch.data_invalid)
//        mBkground_showStackSummary_updateDbData(db_stackSummary);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A U X I L I A R
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t get_db_size(void)
{
    uint8_t db_size = 0;
    uint8_t num_ic;
    uint8_t stack_LU;

    if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
    {
        num_ic = BOARD_get_num_ic_currentboard();

        if(select_2_12V[num_ic] == 2)
            db_size = NUM_CELL_2V_MAX;
        else
            db_size = NUM_CELL_12V_MAX;
    }
    else if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
    {
        db_size=0;
        for (stack_LU=StackLower; stack_LU<=StackUpper; stack_LU++)
        {
            num_ic = board[num_board].stack[stack_LU].address;

            if(select_2_12V[num_ic] == 2)
                db_size += NUM_CELL_2V_MAX;
            else
                db_size += NUM_CELL_12V_MAX;
        }

    }

    return db_size;
}

