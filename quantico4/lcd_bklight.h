
#ifndef LCD_BKLIGHT_H_
#define LCD_BKLIGHT_H_


    #define LCD_PIN_BACKLIGHT 28

    #define LCD_BKLIGHT_ON 1
    #define LCD_BKLIGHT_OFF 0

    void    lcd_bklight_ON(void);
    void    lcd_bklight_OFF(void);
    void    lcd_bklight_timming_reset(void);
    uint8_t lcd_bklight_get_status(void);
    void    lcd_bklight_timming(void);
    uint8_t lcd_bklight_is_ON(void);


    void lcd_bklight_job(void);

    //#define LCD_BACKLIGHT_TIMMING
#endif
