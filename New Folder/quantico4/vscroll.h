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
            struct{
                unsigned mark:1;
                unsigned __a :7;
            }bf;

        }feature;


        uint8_t signal;

        void add_mark(String *pstr);
        void del_mark(String *pstr);
        void clear_sign(void);
        void dispShowBuff_filldata(void);
        void process_signal(void);

    public:
        void init(String *db_data, int16_t db_dbsize, String *dispShowBuff_data, uint8_t  dispShowBuff_numline,
                   uint8_t mark
                  );
        uint16_t get_markPosc(void);
        void sign_up(void);
        void sign_down(void);
        void job(void);

        void feat_mark_ON(void);
        void feat_mark_OFF(void);
        //void del_mark_atPosc(void);

        void set_db(String *db_data);
    };

    extern VScroll vscroll;
#endif



