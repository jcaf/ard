#include <Arduino.h>
#include "menu_stackInDetail_E.h"
#include "system.h"
#include "vscroll.h"
#include "lcd_fxaux.h"

#include "menu.h"
#include "menu_bkground.h"

#include "ikey.h"
#include "kb.h"
#include "LTC68042.h"
#include "ltc6804.h"
#include "stackError.h"
#include "getString.h"
#include "menu_mainMenu.h"


#define STACKINDETAIL_NUMOPT 5

String db_stackInDetail_E[STACKINDETAIL_NUMOPT];//24 cell
VScroll VS_stackInDetail_E;

static inline void stackInDetail_E_fxKey0(void)
{
    scheduler.sm0 = SCHEDULER_MAIN_MENU;
    mainMenuSummary.sm0 = 0;//reinit
}
static inline void stackInDetail_E_fxKey1(void)
{
    VS_stackInDetail_E.sign_up();
}
static inline void stackInDetail_E_fxKey2(void)
{
    VS_stackInDetail_E.sign_down();
}
static inline void stackInDetail_E_fxKey3(void)
{
    int8_t posc = VS_stackInDetail_E.get_markPosc();
    if ( (posc >= 0) && (posc <=2) )
    {
        stackInDetail_E.sm1 = posc+1;//sm1 = 0 -> presenta resumen de flags
        taskman.sign = TASKMAN_SIGN_ENTER;
    }

    //note: options 3 - 4 don't need sub-menus

}
static inline void stackInDetail_E_fxKey4(void)
{
    kb_stackPop();
    taskman.stackSummary.sm0--;
    taskman.sign = TASKMAN_SIGN_EXIT;
}

PTRFX_retVOID const stackInDetail_E_fxKey[KB_NUM_KEYS] PROGMEM =
{stackInDetail_E_fxKey0, stackInDetail_E_fxKey1, stackInDetail_E_fxKey2, stackInDetail_E_fxKey3, stackInDetail_E_fxKey4};

static void stackInDetail_E_updateDbData(String *db);

struct _stackInDetail_E stackInDetail_E;

void stackInDetail_E_job(void)
{
    if(stackInDetail_E.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(stackInDetail_E_fxKey);

        stackInDetail_E_updateDbData(db_stackInDetail_E);

        VS_stackInDetail_E.init(db_stackInDetail_E, STACKINDETAIL_NUMOPT, dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON);

        stackInDetail_E.sm0 = -1;
    }

    if(1) //cualquier cambio en board, se regresa al menu Summary
    {
        VS_stackInDetail_E.job();
        lcd_show_disp();

        if(!ltc6804.app.synch.data_invalid)
            stackInDetail_E_updateDbData(db_stackInDetail_E);
    }
}

static void stackInDetail_E_updateDbData(String *db)
{
    uint8_t num_ic;


    if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
    {
        num_ic = BOARD_get_num_ic_currentboard();

        db[0] = str_clearPrint(getString_error("Under Volt= ", get_stackError_UV(num_ic)), 3);
        db[1] = str_clearPrint(getString_error("Over Volt= ", get_stackError_OV(num_ic)), 3);
        db[2] = str_clearPrint(getString_error("Open Wire= ", get_stackError_openWire(num_ic)), 3);
        db[3] = str_clearPrint(getString_error("Over Current= ", get_stackError_overCurrent(num_ic)), 3);
        db[4] = str_clearPrint(getString_error("Over Temp= ", get_stackError_overTemperature(num_ic)), 3);

    }
    else if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
    {
        //C1...CX - CX+1...CX+n

        uint8_t stackLowerAddr = board[num_board].stack[StackLower].address;
        uint8_t stackUpperAddr = board[num_board].stack[StackUpper].address;
        uint8_t errorStackLower , errorStackUpper;

        errorStackLower = get_stackError_UV(stackLowerAddr);
        errorStackUpper = get_stackError_UV(stackUpperAddr);
        db[0] = str_clearPrint(getString_error("Under Volt= ", errorStackLower+errorStackUpper ), 3);

        errorStackLower = get_stackError_OV(stackLowerAddr);
        errorStackUpper = get_stackError_OV(stackUpperAddr);
        db[1] = str_clearPrint(getString_error("Over Volt= ", errorStackLower+errorStackUpper ), 3);

        errorStackLower = get_stackError_openWire(stackLowerAddr);
        errorStackUpper = get_stackError_openWire(stackUpperAddr);
        db[2] = str_clearPrint(getString_error("Open Wire= ", errorStackLower+errorStackUpper ), 3);

        errorStackLower = get_stackError_overCurrent(stackLowerAddr);
        errorStackUpper = get_stackError_overCurrent(stackUpperAddr);
        db[3] = str_clearPrint(getString_error("Over Current= ", errorStackLower+errorStackUpper ), 3);

        errorStackLower = get_stackError_overTemperature(stackLowerAddr);
        errorStackUpper = get_stackError_overTemperature(stackUpperAddr);
        db[4] = str_clearPrint(getString_error("Over Temp= ", errorStackLower+errorStackUpper ), 3);


    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
VScroll VS_stackInDetail_E_UnderVolt;


static inline void stackInDetail_E_UnderVolt_fxKey0(void)
{
    scheduler.sm0 = SCHEDULER_MAIN_MENU;
    mainMenuSummary.sm0 = 0;//reinit
}
static inline void stackInDetail_E_UnderVolt_fxKey1(void)
{
    VS_stackInDetail_E_UnderVolt.sign_up();
}
static inline void stackInDetail_E_UnderVolt_fxKey2(void)
{
    VS_stackInDetail_E_UnderVolt.sign_down();
}
static inline void stackInDetail_E_UnderVolt_fxKey3(void)
{
    //taskman.sign = TASKMAN_SIGN_ENTER;
}
static inline void stackInDetail_E_UnderVolt_fxKey4(void)
{
    kb_stackPop();
    stackInDetail_E.sm1 = 0;//return to root
    taskman.sign = TASKMAN_SIGN_EXIT;
}

PTRFX_retVOID const stackInDetail_E_UnderVolt_fxKey[KB_NUM_KEYS] PROGMEM =
{stackInDetail_E_UnderVolt_fxKey0, stackInDetail_E_UnderVolt_fxKey1, stackInDetail_E_UnderVolt_fxKey2,
 stackInDetail_E_UnderVolt_fxKey3, stackInDetail_E_UnderVolt_fxKey4};


struct _stackInDetail_E_UnderVolt stackInDetail_E_UnderVolt;
static void stackInDetail_E_UnderVolt_updateDbData(String *db);


void stackInDetail_E_UnderVolt_job(void)
{
    static String db_stackInDetail_E_UnderVolt[LTC_NUM_CELLS*2];//max
    uint8_t db_size;
    uint8_t num_ic = BOARD_get_num_ic_currentboard();

    if (stackInDetail_E_UnderVolt.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(stackInDetail_E_UnderVolt_fxKey);

        db_size = get_db_size();

        stackInDetail_E_UnderVolt_updateDbData(db_stackInDetail_E_UnderVolt);
        VS_stackInDetail_E_UnderVolt.init(db_stackInDetail_E_UnderVolt, db_size , dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON);

        stackInDetail_E_UnderVolt.sm0 = -1;
    }

    if(1) //cualquier cambio en board, se regresa al menu Summary
    {
        VS_stackInDetail_E_UnderVolt.job();
        lcd_show_disp();

        if(!ltc6804.app.synch.data_invalid)
            stackInDetail_E_UnderVolt_updateDbData(db_stackInDetail_E_UnderVolt);
    }
}

static void stackInDetail_E_UnderVolt_updateDbData(String *db)
{
    String str;
    uint8_t num_ic, num_cell, NUM_CELL_TOP;
    uint8_t cv, stack_LU;

    if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
    {
        num_ic = BOARD_get_num_ic_currentboard();

        if(select_2_12V[num_ic] == 2)
            NUM_CELL_TOP = NUM_CELL_2V_MAX;
        else
            NUM_CELL_TOP = NUM_CELL_12V_MAX ;


        for (uint8_t num_cell=0; num_cell<NUM_CELL_TOP; num_cell++)
        {
            str = "C" + String(num_cell+1) +" uV= "+ String(uv_flag[num_ic][num_cell]);
            db[num_cell] = str_clearPrint(str,5);
        }
    }
    else if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
    {
        //C1...CX - CX+1...CX+n
        cv=0;
        for (stack_LU=StackLower; stack_LU<=StackUpper; stack_LU++)
        {
            num_ic = board[num_board].stack[stack_LU].address;

            if(select_2_12V[num_ic] == 2)
                NUM_CELL_TOP = NUM_CELL_2V_MAX;
            else
                NUM_CELL_TOP = NUM_CELL_12V_MAX ;

            for (num_cell=0; num_cell<NUM_CELL_TOP; num_cell++)
            {
                str = "C" + String(cv+1) +" uV= "+ String(uv_flag[num_ic][num_cell]);
                db[cv] = str_clearPrint(str,5);
                cv++;
            }
        }

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
VScroll VS_stackInDetail_E_OverVolt;

static inline void stackInDetail_E_OverVolt_fxKey0(void)
{
    scheduler.sm0 = SCHEDULER_MAIN_MENU;
    mainMenuSummary.sm0 = 0;//reinit
}
static inline void stackInDetail_E_OverVolt_fxKey1(void)
{
    VS_stackInDetail_E_OverVolt.sign_up();
}
static inline void stackInDetail_E_OverVolt_fxKey2(void)
{
    VS_stackInDetail_E_OverVolt.sign_down();
}
static inline void stackInDetail_E_OverVolt_fxKey3(void)
{
    //taskman.sign = TASKMAN_SIGN_ENTER;
}
static inline void stackInDetail_E_OverVolt_fxKey4(void)
{
    kb_stackPop();
    stackInDetail_E.sm1 = 0;//return to root
    taskman.sign = TASKMAN_SIGN_EXIT;
}

PTRFX_retVOID const stackInDetail_E_OverVolt_fxKey[KB_NUM_KEYS] PROGMEM =
{stackInDetail_E_OverVolt_fxKey0, stackInDetail_E_OverVolt_fxKey1, stackInDetail_E_OverVolt_fxKey2,
 stackInDetail_E_OverVolt_fxKey3, stackInDetail_E_OverVolt_fxKey4};

struct _stackInDetail_E_OverVolt stackInDetail_E_OverVolt;
static void stackInDetail_E_OverVolt_updateDbData(String *db);

void stackInDetail_E_OverVolt_job(void)
{
    static String db_stackInDetail_E_OverVolt[LTC_NUM_CELLS*2];//max
    uint8_t db_size;
    uint8_t num_ic = BOARD_get_num_ic_currentboard();

    if (stackInDetail_E_OverVolt.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(stackInDetail_E_OverVolt_fxKey);

        stackInDetail_E_OverVolt_updateDbData(db_stackInDetail_E_OverVolt);

        db_size = get_db_size();
        VS_stackInDetail_E_OverVolt.init(db_stackInDetail_E_OverVolt, db_size , dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON);

        stackInDetail_E_OverVolt.sm0 = -1;
    }

    if(1) //cualquier cambio en board, se regresa al menu Summary
    {
        VS_stackInDetail_E_OverVolt.job();
        lcd_show_disp();

        if(!ltc6804.app.synch.data_invalid)
            stackInDetail_E_OverVolt_updateDbData(db_stackInDetail_E_OverVolt);
    }
}

static void stackInDetail_E_OverVolt_updateDbData(String *db)
{
    String str;
    uint8_t num_ic, num_cell, NUM_CELL_TOP;
    uint8_t cv, stack_LU;

    if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
    {
        num_ic = BOARD_get_num_ic_currentboard();

        if(select_2_12V[num_ic] == 2)
            NUM_CELL_TOP = NUM_CELL_2V_MAX;
        else
            NUM_CELL_TOP = NUM_CELL_12V_MAX ;


        for (num_cell=0; num_cell<NUM_CELL_TOP; num_cell++)
        {
            str = "C" + String(num_cell+1) +" oV= "+ String(ov_flag[num_ic][num_cell]);
            db[num_cell] = str_clearPrint(str,5);
        }
    }
    else if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
    {
        //C1...CX - CX+1...CX+n
        cv=0;
        for (stack_LU=StackLower; stack_LU<=StackUpper; stack_LU++)
        {
            num_ic = board[num_board].stack[stack_LU].address;

            if(select_2_12V[num_ic] == 2)
                NUM_CELL_TOP = NUM_CELL_2V_MAX;
            else
                NUM_CELL_TOP = NUM_CELL_12V_MAX ;

            for (num_cell=0; num_cell<NUM_CELL_TOP; num_cell++)
            {
                str = "C" + String(cv+1) +" oV= "+ String(ov_flag[num_ic][num_cell]);
                db[cv] = str_clearPrint(str,5);
                cv++;
            }
        }

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
VScroll VS_stackInDetail_E_OpenWire;

static inline void stackInDetail_E_OpenWire_fxKey0(void)
{
    scheduler.sm0 = SCHEDULER_MAIN_MENU;
    mainMenuSummary.sm0 = 0;//reinit
}
static inline void stackInDetail_E_OpenWire_fxKey1(void)
{
    VS_stackInDetail_E_OpenWire.sign_up();
}
static inline void stackInDetail_E_OpenWire_fxKey2(void)
{
    VS_stackInDetail_E_OpenWire.sign_down();
}
static inline void stackInDetail_E_OpenWire_fxKey3(void)
{
    //taskman.sign = TASKMAN_SIGN_ENTER;
}
static inline void stackInDetail_E_OpenWire_fxKey4(void)
{
    kb_stackPop();
    stackInDetail_E.sm1 = 0;//return to root
    taskman.sign = TASKMAN_SIGN_EXIT;
}

PTRFX_retVOID const stackInDetail_E_OpenWire_fxKey[KB_NUM_KEYS] PROGMEM =
{stackInDetail_E_OpenWire_fxKey0, stackInDetail_E_OpenWire_fxKey1, stackInDetail_E_OpenWire_fxKey2,
 stackInDetail_E_OpenWire_fxKey3, stackInDetail_E_OpenWire_fxKey4};

struct _stackInDetail_E_OpenWire stackInDetail_E_OpenWire;
static void stackInDetail_E_OpenWire_updateDbData(String *db);

void stackInDetail_E_OpenWire_job(void)
{
    static String db_stackInDetail_E_OpenWire[LTC_NUM_CELLS*2];//max
    uint8_t db_size;
    uint8_t num_ic;
    uint8_t stack_LU;

    if (stackInDetail_E_OpenWire.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(stackInDetail_E_OpenWire_fxKey);

        stackInDetail_E_OpenWire_updateDbData(db_stackInDetail_E_OpenWire);

        db_size = get_db_size();
        VS_stackInDetail_E_OpenWire.init(db_stackInDetail_E_OpenWire, db_size , dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON);

        stackInDetail_E_OpenWire.sm0 = -1;
    }

    if(1) //cualquier cambio en board, se regresa al menu Summary
    {
        VS_stackInDetail_E_OpenWire.job();
        lcd_show_disp();

        if(!ltc6804.app.synch.data_invalid)
            stackInDetail_E_OpenWire_updateDbData(db_stackInDetail_E_OpenWire);
    }
}

static void stackInDetail_E_OpenWire_updateDbData(String *db)
{
    String str;
    uint8_t num_ic, num_cell, NUM_CELL_TOP;
    uint8_t cv, stack_LU;


    if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
    {
        num_ic = BOARD_get_num_ic_currentboard();

        if(select_2_12V[num_ic] == 2)
            NUM_CELL_TOP = NUM_CELL_2V_MAX;
        else
            NUM_CELL_TOP = NUM_CELL_12V_MAX ;


        for (num_cell=0; num_cell<NUM_CELL_TOP; num_cell++)
        {
            str = "C" + String(num_cell+1) +" OpenW= "+ String(openwire_flag[num_ic][num_cell]);
            db[num_cell] = str_clearPrint(str,4);
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
                str = "C" + String(cv+1) +" OpenW= "+ String(openwire_flag[num_ic][num_cell]);
                db[cv] = str_clearPrint(str,4);
                cv++;
            }
        }

    }
}

