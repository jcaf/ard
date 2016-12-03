#ifndef LCD_FXAUX_H_
#define LCD_FXAUX_H_

    #define LCD_ROW 4
	#define LCD_COL 20

	#define LCD_ROW_0 0
	#define LCD_ROW_1 1
	#define LCD_ROW_2 2
	#define LCD_ROW_3 3

    //void lcd_show_disp(void);
    void lcd_show_disp(uint8_t fil_ini=LCD_ROW_1, uint8_t fil_end=LCD_ROW_3);


    String str_clearPrint(String pstr,  uint8_t posc_i=0);

    #include <LiquidCrystal.h>
    class mylcd: public LiquidCrystal
    {
        using LiquidCrystal::LiquidCrystal;

        public:
        void printAtPosc(uint8_t fil, uint8_t col, String s);
        void printTrimmReplaceByDot(uint8_t fil, uint8_t col, String s);
        void clearRows4x20(int8_t row0=-1, int8_t row1=-1, int8_t row2=-1, int8_t row3=-1);

    };

    void lcd_print_at_posc(uint8_t fil, uint8_t col, String s);
    void lcd_print_trimmReplacebydot(uint8_t fil, uint8_t col, String s);


    #define STR_CENTER -1
    uint8_t str_center(String str);

#endif
