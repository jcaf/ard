#ifndef VSCROLL_H_
#define VSCROLL_H_

#define SIGN_CLEAR 0
#define SIGN_UP 1
#define SIGN_DOWN 2

#define FEAT_MARK_OFF 0
#define FEAT_MARK_ON 1

class VScroll
{
private:
    struct
    {
        String *data;
        int16_t dbsize;
        int16_t offset;
        int16_t base;
    } db;

    struct
    {
        String *data;
        uint8_t  numline;
    } dispShowBuff;

    struct
    {
        struct
        {
            unsigned mark:1;
            unsigned __a :7;
        } bf;

    } feature;

    uint8_t signal;
    void add_mark(String *pstr);
    void del_mark(String *pstr);
    void clear_sign(void);
    void dispShowBuff_filldata(void);
    void process_signal(void);

    #define VSCROLL_STACK_DEEP 7
    struct _vscrollst
    {
        struct _stack
        {
            int16_t offset;
            int16_t base;
        } stack[VSCROLL_STACK_DEEP];

        int8_t sp;
    } vscrollst;

public:
    void init(String *db_data, int16_t db_dbsize, String *dispShowBuff_data, uint8_t  dispShowBuff_numline,
              uint8_t mark,
              int16_t base=0,
              int16_t offset=0
             );
    uint16_t get_markPosc(void);
    void sign_up(void);
    void sign_down(void);
    void job(void);
    void feat_mark_ON(void);
    void feat_mark_OFF(void);
    //void del_mark_atPosc(void);
    void set_db(String *db_data);
    //
    int16_t offset;
    int16_t base;
    void stack_push(void);
    void stack_pop(uint8_t n=1);//1 pop x default
    void base_offset_reset(void);
};

extern VScroll vscroll;

#define DISP_SHOW_BUFF_LENGTH 3
extern String dispShowBuff[DISP_SHOW_BUFF_LENGTH];

#endif




