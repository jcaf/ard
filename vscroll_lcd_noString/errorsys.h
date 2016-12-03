#ifndef ERRORSYS_H_
#define ERRORSYS_H_

    void error_description(const char *code, char *buff_desc);
    uint8_t error_mgm(long _delay, uint8_t autoerase, const char *code, const char *param0=NULL);
    //uint8_t error_mgm(long _delay, uint8_t autoerase, const char *code, const char *param0);

    #define ERROR_LCD_AUTOERASE_ON 1
    #define ERROR_LCD_AUTOERASE_OFF 0

#endif // ERRORSYS_H_


