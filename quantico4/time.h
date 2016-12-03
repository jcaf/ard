#ifndef TIME_H_
#define TIME_H_

void mainMenu_setTime_job(void);

    struct _taskman_setTime
    {
        int8_t sm0;
        int8_t sign;
        int8_t reInitJob;

    };
    extern struct _taskman_setTime taskman_setTime;

//struct _menuSetTime
//{
//    int8_t sm0;
//};
//
//
//struct _menuSetTime_save
//{
//    int8_t sm0;
//};


#endif // TIME_H_
