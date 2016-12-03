#ifndef MENU_INPUTS_H_
#define MENU_INPUTS_H_

    void mainMenu_inputs_job(void);


    struct _taskman_input
    {
        int8_t sm0;
        int8_t sign;
        int8_t reInitJob ;

    };
    extern struct _taskman_input taskman_input;

    struct _menuInputSummary
    {
        int8_t sm0;
    };
    extern struct _menuInputSummary menuInputSummary;


    struct _menu_digitalinput
    {
        int8_t sm0;
    };
    extern struct _menu_digitalinput menu_digitalinput;

#endif // MENU_INPUTS_H_
