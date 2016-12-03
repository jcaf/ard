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
#include "menu_stackInDetail_V.h"
#include "menu_stackInDetail_E.h"
#include "menu_mainMenu.h"

static void stackSummary_job(void);
static void stackInsideSummary_job(void);

#define STACKSUMMARY        0
#define STACKINSIDESUMMARY  1
#define STACKINDETAIL       2

#define STACKINDETAIL_V 0
#define STACKINDETAIL_I 1
#define STACKINDETAIL_T 2
#define STACKINDETAIL_E 3

struct _taskman taskman;

struct _stackSummary
{
    int8_t sm0;
} stackSummary;

struct _stackInsideSummary
{
    int8_t sm0;
} stackInsideSummary;

static void board_setText(void);

void menuBkGround_job(void)
{
    //BOARDd();

    if (boardD.set_text)//show again on-demand
    {
        boardD.set_text = 0;
        board_setText();
//        //+-add
//        if (taskman.stackSummary.sm0 == STACKSUMMARY)
//            {lcd.setCursor(18,0);lcd.print("<>");}
//        //-+

    }


    if(boardD.AchangeInBoard)//reset all smX
    {
        board_setText();

        if (stackInDetail_E.sm1>0)
        {
             do
            {
                stackInDetail_E.sm1--;
            }
            while(stackInDetail_E.sm1 > 0);

            kb_stackPop();  //todas las opciones hicieron 1 push()
        }

        if(taskman.stackSummary.sm0 > 0)
        {
            do
            {
                kb_stackPop();
                taskman.stackSummary.sm0--;
            }
            while(taskman.stackSummary.sm0 > 0);//return to taskman.stackSummary.sm0 = 0
                                                //stackSummary_job no entra con SIGN_ENTER por defecto, es la app quien lo pide
        }
    }

    if (taskman.sign  == TASKMAN_SIGN_ENTER)
    {
        taskman.stackSummary.reInitJob = 1;
    }
                                                        //    if(taskman.sign  == TASKMAN_SIGN_EXIT)
                                                        //    {
                                                        //    }

    //+-Scheduling
    if (taskman.stackSummary.sm0 == STACKSUMMARY) //0
    {
        if(taskman.stackSummary.reInitJob == 1)
            stackSummary.sm0 = 0;//re-init

        stackSummary_job();

    }
    else if(taskman.stackSummary.sm0 == STACKINSIDESUMMARY) //1
    {
        if(taskman.stackSummary.reInitJob == 1)
            stackInsideSummary.sm0 = 0;//re-init

        stackInsideSummary_job();
    }
    else if(taskman.stackSummary.sm0 == STACKINDETAIL) //2
    {
        if(taskman.stackInDetail.sm0 == STACKINDETAIL_V)
        {
            if(taskman.stackSummary.reInitJob == 1)
                stackInDetail_V.sm0 = 0;//re-init

            stackInDetail_V_job();
        }
        else if(taskman.stackInDetail.sm0 == STACKINDETAIL_I)
        {
            //            if (taskman.stackSummary.reInitJob == 1)
            //                stackInDetail_I.sm0 = 0;//re-init

            //            stackInDetail_I_job();
        }
        else if(taskman.stackInDetail.sm0 == STACKINDETAIL_T)
        {
            //            if (taskman.stackSummary.reInitJob == 1)
            //                stackInDetail_T.sm0 = 0;//re-init

            //            stackInDetail_T_job();
        }
        else if(taskman.stackInDetail.sm0 == STACKINDETAIL_E)
        {
            if(stackInDetail_E.sm1 == 0)
            {
                if(taskman.stackSummary.reInitJob == 1)
                    stackInDetail_E.sm0 = 0;

                stackInDetail_E_job();
            }
            else if(stackInDetail_E.sm1 == 1)
            {
                if(taskman.stackSummary.reInitJob == 1)
                    stackInDetail_E_UnderVolt.sm0 = 0;

                stackInDetail_E_UnderVolt_job();
            }
            else if(stackInDetail_E.sm1 == 2)
            {
                if(taskman.stackSummary.reInitJob == 1)
                    stackInDetail_E_OverVolt.sm0 = 0;

                stackInDetail_E_OverVolt_job();
            }
            else if(stackInDetail_E.sm1 == 3)
            {
                if(taskman.stackSummary.reInitJob == 1)
                    stackInDetail_E_OpenWire.sm0 = 0;

                stackInDetail_E_OpenWire_job();
            }
            else if(stackInDetail_E.sm1 == 4)
            {
//                if(taskman.stackSummary.reInitJob == 1)
//                    stackInDetail_E_OverCurrent.sm0 = 0;
//
//                stackInDetail_E_OverCurrent_job();
            }
            else if(stackInDetail_E.sm1 == 5)
            {
//                if(taskman.stackSummary.reInitJob == 1)
//                    stackInDetail_E_OverTemp.sm0 = 0;
//
//                stackInDetail_E_OverTemp_job();
            }
        }
    }

    if(taskman.stackSummary.reInitJob == 1)
        taskman.stackSummary.reInitJob = 0;

    if(taskman.sign != TASKMAN_SIGN_CLEAR)
        taskman.sign = TASKMAN_SIGN_CLEAR;

    if(boardD.AchangeInBoard == 1)
        boardD.AchangeInBoard = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void board_setText(void)
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
    if (taskman.stackSummary.sm0 == STACKSUMMARY)
        {lcd.setCursor(18,0);lcd.print("<>");}
    //-+
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
String dispShowBuff[DISP_SHOW_BUFF_LENGTH];
static void stackSummary_updateDbData(void);


static void stackSummary_fxKey0(void)//Menu
{
    scheduler.sm0 = SCHEDULER_MAIN_MENU;

    mainMenuSummary.sm0 = 0;//reinit
}
static void stackSummary_fxKey1(void)//Up
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
static void stackSummary_fxKey2(void)//Down
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
static void stackSummary_fxKey3(void)//Enter
{
    uint8_t opt = board[num_board].stacks.num_stack;

    if(ltc_connected[ board[num_board].stack[opt].address] == LTC_CONNECTED)
    {
        taskman.sign = TASKMAN_SIGN_ENTER;

        //entra a modo vscrool con lo que muestra actualmente
        taskman.stackSummary.sm0 = STACKINSIDESUMMARY;

        lcd.setCursor(18,0);lcd.print("  ");//clear "<>"
    }
}
static void stackSummary_fxKey4(void)//eScape
{
}

PTRFX_retVOID const stackSummary_fxKey[KB_NUM_KEYS] PROGMEM=
{stackSummary_fxKey0, stackSummary_fxKey1, stackSummary_fxKey2, stackSummary_fxKey3, stackSummary_fxKey4};

static void updateDbData(void);

#define STACK_SUMMARY_NUM_ELEM 4
String db_stackSummary[STACK_SUMMARY_NUM_ELEM];
VScroll VS_stackSummary;

static void stackSummary_job(void)
{
    static uint8_t doThisJob;

    if (stackSummary.sm0 == 0) //init
    {
        //kb_stackPush();
        kb_change_keyDo_pgm(stackSummary_fxKey);

        stackSummary.sm0 = -1;
    }

    if(boardD.AchangeInBoard)
    {
        if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
        {
            VS_stackSummary.init(db_stackSummary, STACK_SUMMARY_NUM_ELEM, dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_OFF);
            doThisJob = 1;
        }
        else  if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
        {
            //S = # de stack
            uint8_t opt = board[num_board].stacks.num_stack;

            if(ltc_connected[ board[num_board].stack[opt].address] == LTC_CONNECTED)
            {
                VS_stackSummary.init(db_stackSummary, STACK_SUMMARY_NUM_ELEM, dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_OFF);
                doThisJob = 1;
            }
            else
                doThisJob = 0;
        }
    }

    if(doThisJob)
    {
        VS_stackSummary.job();
        lcd_show_disp();

        if(!ltc6804.app.synch.data_invalid)
            updateDbData();
    }
}

static void updateDbData(void)
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
static void stackInsideSummary_fxKey0(void)
{
    scheduler.sm0 = SCHEDULER_MAIN_MENU;
    mainMenuSummary.sm0 = 0;//reinit
}
static void stackInsideSummary_fxKey1(void)
{
    VS_stackSummary.sign_up();
}
static void stackInsideSummary_fxKey2(void)
{
    VS_stackSummary.sign_down();
}
static void stackInsideSummary_fxKey3(void)
{
    taskman.stackSummary.sm0 = STACKINDETAIL;
    /*
    STACK_INDETAIL posc = VS_stackSummary.get_markPosc();

    if (posc == SUMMARY_V)//voltage is <<marked
    {
         taskman.stackInDetail.sm0 = STACKINDETAIL_V;
    }
    if (posc == SUMMARY_I)//current is <<marked
    {
        //taskman.stackInDetail.sm0 = STACKINDETAIL_I;
    }
    if (posc == SUMMARY_T)//temperature is <<marked
    {
        //taskman.stackInDetail.sm0 = STACKINDETAIL_T;
    }
    if (posc == SUMMARY_E)//error is <<marked
    {
        taskman.stackInDetail.sm0 = STACKINDETAIL_E;
    }
    */
    taskman.stackInDetail.sm0 = VS_stackSummary.get_markPosc();

    taskman.sign = TASKMAN_SIGN_ENTER;
}
void stackInsideSummary_fxKey4(void)
{
    kb_stackPop();
    taskman.stackSummary.sm0--;
    taskman.sign = TASKMAN_SIGN_EXIT;
    //
    //VS_stackSummary.del_mark_atPosc();
    VS_stackSummary.feat_mark_OFF();
    //+-add
    //if (taskman.stackSummary.sm0 == STACKSUMMARY)
        {lcd.setCursor(18,0);lcd.print("<>");}
    //-+
}

PTRFX_retVOID const stackInsideSummary_fxKey[KB_NUM_KEYS] PROGMEM=
{stackInsideSummary_fxKey0, stackInsideSummary_fxKey1, stackInsideSummary_fxKey2, stackInsideSummary_fxKey3, stackInsideSummary_fxKey4};

void stackInsideSummary_job(void)
{
    if(stackInsideSummary.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(stackInsideSummary_fxKey);

        VS_stackSummary.feat_mark_ON();

        stackInsideSummary.sm0 = -1;
    }

    if(1)
    {
        VS_stackSummary.job();
        lcd_show_disp();

        if(!ltc6804.app.synch.data_invalid)
            updateDbData();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A U X I L I A R
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t get_db_size(void)
{
    uint8_t db_size, num_ic, stack_LU;

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
