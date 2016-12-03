#ifndef MENU_BKGROUND_H_
#define MENU_BKGROUND_H_

//    #define TASKMAN_SIGN_CLEAR 0
//    #define TASKMAN_SIGN_ENTER 1
//    #define TASKMAN_SIGN_EXIT 2

    struct _taskman
    {
        struct
        {
            //STACK_SUMMARY
            int8_t sm0;
            int8_t reInitJob;//sm1;
        } stackSummary;

        struct
        {
            //STACK_INDETAIL
            int8_t sm0;
            int8_t sm1;
        } stackInDetail;

        int8_t sign;
    };
    extern struct _taskman taskman;

    void menuBkGround_job(void);




    String getString_error(String Text, uint8_t E);

    //AUXILIAR
    uint8_t get_db_size(void);

#endif


