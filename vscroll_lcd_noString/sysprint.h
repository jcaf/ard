#ifndef SYSPRINT_H_
#define SYSPRINT_H_

#define SYSPRINT_STR_MAX_SIZE 40

void sysPrint_serialPrint(const char *str);
void sysPrint_serialPrint_P(const char *str);
void sysPrint_serialPrintln(const char *str);
void sysPrint_serialPrintln_P( const char *str);

void sysPrintLCDan_XY(uint8_t fil, int8_t col, const char *str);
void sysPrintLCDan_XY_wlineClr(uint8_t fil, int8_t col, const char *str_src);
void sysPrintLCDan_XY_wlineClr_wEOL3dots(uint8_t fil, int8_t col, const char *str_src);
void sysPrintLCDan_XY_wlineUnclr(int8_t fil, int8_t col_i, const char *str_src);
void sysPrintLCDan_XY_wlineUnclr_wEOL3dots(int8_t fil, int8_t col_i, const char *str_src);

void sysPrintLCDan_XY_P(uint8_t fil, int8_t col, const char *str);
void sysPrintLCDan_XY_wlineClr_P(uint8_t fil, int8_t col, const char *str_src);
void sysPrintLCDan_XY_wlineClr_wEOL3dots_P(uint8_t fil, int8_t col, const char *str_src);
void sysPrintLCDan_XY_wlineUnclr_P(int8_t fil, int8_t col_i, const char *str_src);
void sysPrintLCDan_XY_wlineUnclr_wEOL3dots_P(int8_t fil, int8_t col_i, const char *str_src);

void sysPrint_lcd_serialPrintln(uint8_t fil, int8_t col, char *str);
void sysPrint_lcd_serialPrintln_P(uint8_t fil, int8_t col, const char *str);


#endif

