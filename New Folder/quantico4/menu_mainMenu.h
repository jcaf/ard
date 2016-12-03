#ifndef MENU_MAINMENU_H_
#define MENU_MAINMENU_H_

    #define SCHEDULER_BK_GROUND 0
    #define SCHEDULER_MAIN_MENU 1

//    struct _main_menu
//    {
//        int8_t sm0;
//    };
//    extern struct _main_menu main_menu;



    void main_menu_job(void);

    struct _mainMenuSummary
    {
        int8_t sm0;
    };


    extern struct _mainMenuSummary mainMenuSummary;

//++-
struct _taskman_mainMenu
{
//    struct
//    {
//        int8_t sm0;
//        int8_t reInitJob;
//    }sys_error;
//
//    struct
//    {
//        int8_t sm0;
//        int8_t sm1;
//    }contact;
//
//    struct
//    {
//        int8_t sm0;
//        int8_t sm1;
//    }information;
//
//    struct
//    {
//        int8_t sm0;
//        int8_t sm1;
//    }inputs;
//
//    struct
//    {
//        int8_t sm0;
//        int8_t sm1;
//    }outputs;


    int8_t sm0;
    //int8_t sign;
};
extern struct _taskman_mainMenu taskman_mainMenu;
//-++

#endif // MENU_MAINMENU_H_





