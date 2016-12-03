#ifndef BOOT_H_
#define BOOT_H_
    uint8_t sdcard_init(void);
    void sdcard_runtest_H10(void);
    uint8_t system_set_functionmode(void);

    int8_t mainvar_isFound(String setting_name);
    String  mainvar_getvalue(String setting_name);
    uint8_t mainvar_setvalue(String setting_name, String setting_value);


    //extern struct _mainvar mainvar[];


#endif // BOOT_H_
