#ifndef ERRORSYS_H_
#define ERRORSYS_H_

//    #define ERRORSYS_CODE_MAXCHAR 10//6+1
//    #define ERRORSYS_DESC_MAXCHAR 50

//    struct _errorsys_db
//    {
//        const char code[ERRORSYS_CODE_MAXCHAR];
//        const char desc[ERRORSYS_DESC_MAXCHAR];
//    };


    //String error_description(const char* code);
    void error_description(const char* code, char *buff_desc);


    //uint8_t error_mgm(const char *code, String param0="");
    uint8_t error_mgm(long _delay, uint8_t autoerase, const char *code, String param0="");


    #define ERROR_LCD_AUTOERASE_ON 1
    #define ERROR_LCD_AUTOERASE_OFF 0

#endif // ERRORSYS_H_
