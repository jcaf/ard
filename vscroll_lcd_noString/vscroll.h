#ifndef VSCROLL_H_
#define VSCROLL_H_

#define _VSCROLL_DATATYPE_AS_INT8_T
//#define _VSCROLL_DATATYPE_AS_INT16_T

#ifdef _VSCROLL_DATATYPE_AS_INT8_T
typedef int8_t VSCROLLDT;//DataType
#endif // _VSCROLL_DATATYPE_AS_INT8_T

#ifdef _VSCROLL_DATATYPE_AS_INT16_T
typedef int16_t VSCROLLDT;//DataType
#endif // _VSCROLL_DATATYPE_AS_INT16_T


#define SIGN_CLEAR 0
#define SIGN_UP 1
#define SIGN_DOWN 2

#define FEAT_MARK_OFF 0
#define FEAT_MARK_ON 1


#include "lcd_fxaux.h"

class VScroll
{
private:
    struct
    {
        char (*data)[LCDAN_STR_MAX_SIZE];
        VSCROLLDT dbsize;
        VSCROLLDT offset;
        VSCROLLDT base;
    } db;

    struct
    {
        char (*data)[LCDAN_STR_MAX_SIZE];
        VSCROLLDT  numline;
    } vscroll_dispShowBuff;

    struct
    {
        struct
        {
            unsigned mark:1;
            unsigned __a :7;
        } bf;

    } feature;

    uint8_t signal;
    void add_mark(char *arr);
    void del_mark(char *arr);
    void clear_sign(void);
    void dispShowBuff_filldata(void);
    void process_signal(void);

    #define VSCROLL_STACK_DEEP 7
    struct _vscrollst
    {
        struct _stack
        {
            VSCROLLDT offset;
            VSCROLLDT base;
        } stack[VSCROLL_STACK_DEEP];

        VSCROLLDT sp;
    } vscrollst;

public:
    void init(char (*db_data)[LCDAN_STR_MAX_SIZE],
                   VSCROLLDT db_dbsize,
                   char (*dispShowBuff_data)[LCDAN_STR_MAX_SIZE],
                   VSCROLLDT  dispShowBuff_numline,
                   uint8_t mark,//feature
                   VSCROLLDT base,
                   VSCROLLDT offset
                   );
    VSCROLLDT get_markPosc(void);
    void sign_up(void);
    void sign_down(void);
    void job(void);
    void feat_mark_on(void);
    void feat_mark_off(void);
    void set_db(char (*db_data)[LCDAN_STR_MAX_SIZE]);
    //
    VSCROLLDT offset;
    VSCROLLDT base;
    void stack_push(void);
    void stack_pop(uint8_t n=1);//1 pop x default
    void base_offset_reset(void);

    void set_db_dbsize(VSCROLLDT db_dbsize);
    void set_dispShowBuff_numline(VSCROLLDT  dispShowBuff_numline);
};

extern VScroll vscroll;

#define DISP_SHOW_BUFF_LENGTH 3
extern char vscroll_dispShowBuff[DISP_SHOW_BUFF_LENGTH][LCDAN_STR_MAX_SIZE];

void vscroll_lcd_show_disp(uint8_t fil_ini = LCD_ROW_1, uint8_t fil_end = LCD_ROW_3);

#endif
