#ifndef LCD_FXAUX_H_
#define LCD_FXAUX_H_

//size of this LCD alfan.
#define LCD_ROW 4
#define LCD_COL 20

#define LCDAN_STR_MAX_SIZE (LCD_COL+1)//20 char printable + '\0'

//rows
#define LCD_ROW_0 0
#define LCD_ROW_1 1
#define LCD_ROW_2 2
#define LCD_ROW_3 3


void lcdan_str_lineformat_align(char *str_dest, const char *str_src, int8_t pos_i = 0);
void lcdan_str_lineformat_align_P(char *str_dest, const char *str_src, int8_t pos_i = 0);
void lcdan_str_lineformat_trimEOL3dots(char *str);
uint8_t lcdan_str_get_align_col(const char *str_src, int8_t col_align);
#define LCDAN_STR_FORMAT_ALIGN_CENTER -1
int8_t lcdan_str_get_center_hor(const char *str);

#include <LiquidCrystal.h>
class mylcd : public LiquidCrystal
{
    using LiquidCrystal::LiquidCrystal;//hereda constructor

public:
    void printAtPosc(uint8_t fil, uint8_t col, const char *str);
    void printAtPosc_char(uint8_t fil, uint8_t col, const char c);
    
    //void printTrimmReplaceByDot(uint8_t fil, uint8_t col, char *str);
    void clearRows4x20(int8_t row0 = -1, int8_t row1 = -1, int8_t row2 = -1, int8_t row3 = -1);
};
extern mylcd lcd;

//void lcd_print_at_posc(uint8_t fil, uint8_t col, char *s);
//void lcd_print_trimmReplacebydot(uint8_t fil, uint8_t col, char *s);



#endif


