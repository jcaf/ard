#include <Arduino.h>
#include <LiquidCrystal.h>
#include "system.h"
#include "ikey.h"
#include "kb.h"
#include "lcd_bklight.h"
#include "lcd_fxaux.h"
#include "vscroll.h"
#include "sysprint.h"
#include "sdcard.h"

#include <SPI.h>
#include "boot.h"

//
#include "logicblock.h"

mylcd lcd(22, 23, 24, 25, 26, 27);//LiquidCrystal lcd(22, 23, 24, 25, 26, 27);


int8_t thismenu_sm0;
int8_t thismenu_sm1;
int8_t thismenu_sm2;
int8_t thismenu_sm0_last;
unsigned long thismenu_last_millis0;
int8_t temp0_int8_t;

void setup()
{
  lcd.begin(LCD_COL, LCD_ROW);
  lcd.setCursor(0, 0);
  digitalWrite(LCD_PIN_BACKLIGHT, LOW);
  pinMode(LCD_PIN_BACKLIGHT, OUTPUT);
  lcd_bklight_ON();//lcd_bklight_OFF();

/*
sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0,19, PSTR("JUAN CARLOS"));
sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1,18, PSTR("JUAN CARLOS"));
sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2,17, PSTR("JUAN CARLOS"));
sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_3,9, PSTR("JUAN CARLOS"));//ok
//sysPrintLCDan_XY_wlineClr(LCD_ROW_3,11,"123");//ok
//sysPrintLCDan_XY_wlineUnclr(LCD_ROW_3,19,"123");//ok
sysPrintLCDan_XY_wlineUnclr_wEOL3dots_P(LCD_ROW_3,13, PSTR("12345"));//ok
*/
Serial.begin(230400);

char c='k';
lcd.printAtPosc_char(0, 0, c);
while(1);

sysPrint_lcd_serialPrintln_P(LCD_ROW_0,1, PSTR("JUAN CARLOS"));
sysPrint_lcd_serialPrintln_P(LCD_ROW_1,18, PSTR("JUAN CARLOS"));
sysPrint_lcd_serialPrintln_P(LCD_ROW_2,17, PSTR("JUAN CARLOS"));
sysPrint_lcd_serialPrintln_P(LCD_ROW_3,9, PSTR("JUAN CARLOS"));//ok
sysPrint_lcd_serialPrintln_P(LCD_ROW_3,13, PSTR("12345"));//ok
sysPrintLCDan_XY_wlineClr_P(LCD_ROW_0,0,PSTR(""));//clear Line
while(1);

//////////////////////////////////////////
  //SPI_deselectAllperipherals();
  SPI.begin();

  if (sdcard_init())
    if (system_set_functionmode())
      if (mysystem.funct_mode == FUNCT_MODE_DEBUG)
        Serial.begin(230400);

  sdcard_runtest_H10();

  while(1);
///////////////////////////////////


  vscroll.base_offset_reset();

  //
  key_initialization();
  InitTCNT1();
  TIMSK1 = (1 << OCIE1A);
  //
}

int8_t db_size = 1;
int8_t dispShowBuff_numline = 1;

static inline void mlogb_create_exp_fxKey0(void)
{

  if (db_size < 12)//ojo es tamano, no offset
  {
    db_size++;

    vscroll.set_db_dbsize(db_size);

    //
    if (dispShowBuff_numline < 3)
    {
      dispShowBuff_numline++;
      vscroll.set_dispShowBuff_numline(dispShowBuff_numline);
    }

    vscroll.sign_down();

  }

}
static inline void mlogb_create_exp_fxKey1(void)
{
  vscroll.sign_up();

}
static inline void mlogb_create_exp_fxKey2(void)
{
  vscroll.sign_down();
}

static inline void mlogb_create_exp_fxKey3(void)
{
}
static inline void mlogb_create_exp_fxKey4(void)
{
}

PTRFX_retVOID const mlogb_create_exp_fxKey[KB_NUM_KEYS] PROGMEM =
{
  mlogb_create_exp_fxKey0, mlogb_create_exp_fxKey1, mlogb_create_exp_fxKey2, mlogb_create_exp_fxKey3, mlogb_create_exp_fxKey4
};
void mlogb_create_exp_job(void)
{
  char mlogb_create_exp[12][LCDAN_STR_MAX_SIZE];
  lcdan_str_lineformat_align_P(&mlogb_create_exp[0][0],  PSTR("D1 AND [D2 OR D3]"), 0);
  lcdan_str_lineformat_align_P(&mlogb_create_exp[1][0],  PSTR("D1 && (D2 || D3)"), 0);
  lcdan_str_lineformat_align_P(&mlogb_create_exp[2][0],  PSTR("A1 + 1000.00"), 0);
  lcdan_str_lineformat_align_P(&mlogb_create_exp[3][0],  PSTR("[[A2 > 1000] OR [D1"), 0);
  lcdan_str_lineformat_align_P(&mlogb_create_exp[4][0],  PSTR("((A2 > 1000) OR (D1"), 0);
  lcdan_str_lineformat_align_P(&mlogb_create_exp[5][0],  PSTR("OR D2]] AND A3 >= 4"), 0);
  lcdan_str_lineformat_align_P(&mlogb_create_exp[6][0],  PSTR("=TimerToON[D1,60]"), 0);
  lcdan_str_lineformat_align_P(&mlogb_create_exp[7][0],  PSTR("=TimerToOFF[D2,"), 0);
  lcdan_str_lineformat_align_P(&mlogb_create_exp[8][0],  PSTR("3600]"), 0);
  lcdan_str_lineformat_align_P(&mlogb_create_exp[9][0],  PSTR("=D3(LOW)"), 0);
  lcdan_str_lineformat_align_P(&mlogb_create_exp[10][0], PSTR("=D4[HIGH]"), 0);
  lcdan_str_lineformat_align_P(&mlogb_create_exp[11][0], PSTR("Timer[D1(HIGH),6]"), 0);

  //lcdan_str_lineformat_align(&mlogb_create_exp[0][0],  "YES", 7);//RAM
  //lcdan_str_lineformat_align(&mlogb_create_exp[1][0],  "NO", 7);//RAM

  if (thismenu_sm0 == 0)
  {
    lcd.clear();

    kb_change_keyDo_pgm(mlogb_create_exp_fxKey);
    // vscroll.init(mlogb_create_exp, 2, vscroll_dispShowBuff, 1, FEAT_MARK_ON, 0, 0);
    vscroll.init(mlogb_create_exp, db_size, vscroll_dispShowBuff, dispShowBuff_numline, FEAT_MARK_ON, 0, 0);
    //sysPrint_lcdPrint_P(0,0, PSTR("012345678901234567890123456789"));
    sysPrintLCDan_XY_wlineClr_wEOL3dots(0,0, "012345678901234567890123456789");
    

    thismenu_sm0 = -1;
  }
  /*
  if (thismenu_sm1)
  {
    //vscroll.init(mlogb_create_exp, db_count, vscroll_dispShowBuff, disp_count, FEAT_MARK_ON, 0, 0);
    vscroll.set_db_dbsize();
    vscroll.set_dispShowBuff_numline();
    vscroll.sign_down();
    thismenu_sm1 = 0;
  }
  */

  vscroll.set_db(mlogb_create_exp);
  vscroll.job();

  //USAR:
  vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_1 + (dispShowBuff_numline - 1)); //fil 2 y fil3 are printable
  //o que es MUCHO MEJOR con sentido
  //vscroll_lcd_show_disp(LCD_ROW_2, 2);//cuantas filas se van a imprimir

}

void loop() {
  kb_processKeyRead_D();
  mlogb_create_exp_job();
}

ISR(TIMER1_COMPA_vect)
{
  #ifdef LCD_BACKLIGHT_TIMMING
  lcd_bklight_timming();
  #endif
  kb_job();
}


////////////////////////////////
void logb_symbol_add(const class _symbol &symbol )
{
  static int8_t idx;

  logb_symbol[idx] = symbol;

  idx++;
}

void logb_symbol_get_text(const class _symbol &symbol, char *text_out)
{
  switch (symbol.get_type())
  {
  case LOGB_SYMB_TYPE_DIG_INPUT:
    switch (symbol.val)
    {
    case 0: strcpy_P(text_out, (const char *) PSTR("D1"));
      break;
    case 1: strcpy_P(text_out, (const char *) PSTR("D2"));
      break;
    case 2: strcpy_P(text_out, (const char *) PSTR("D3"));
      break;
    case 3: strcpy_P(text_out, (const char *) PSTR("D4"));
      break;
    }
    break;

  case LOGB_SYMB_TYPE_AN_INPUT:
    switch (symbol.val)
    {
    case 0: strcpy_P(text_out, (const char *) PSTR("A1"));
      break;
    case 1: strcpy_P(text_out, (const char *) PSTR("A2"));
      break;
    case 2: strcpy_P(text_out, (const char *) PSTR("A3"));
      break;
    case 3: strcpy_P(text_out, (const char *) PSTR("A4"));
      break;
    }
    break;

  case LOGB_SYMB_TYPE_RELAY:
    switch (symbol.val)
    {
    case 0: strcpy_P(text_out, (const char *) PSTR("R1"));
      break;
    case 1: strcpy_P(text_out, (const char *) PSTR("R2"));
      break;
    case 2: strcpy_P(text_out, (const char *) PSTR("R3"));
      break;
    case 3: strcpy_P(text_out, (const char *) PSTR("R4"));
      break;
    case 4: strcpy_P(text_out, (const char *) PSTR("R5"));
      break;
    }
    break;
  //
  case LOGB_SYMB_TYPE_LOGICAL_OP:
    switch (symbol.val)
    {
    case LOGB_LOGICOP_AND:  strcpy_P(text_out, (const char *) PSTR("AND"));
      break;
    case LOGB_LOGICOP_OR:   strcpy_P(text_out, (const char *) PSTR("OR"));
      break;
    case LOGB_LOGICOP_NOT:  strcpy_P(text_out, (const char *) PSTR("NOT"));
      break;
    }
    break;

  case LOGB_SYMB_TYPE_RELATIONAL_OP:
    switch (symbol.val)
    {
    case LOGB_RELAT_OP_EQUAL_TO:      strcpy_P(text_out, (const char *) PSTR("=="));
      break;
    case LOGB_RELAT_OP_NOTEQUAL_TO:   strcpy_P(text_out, (const char *) PSTR("!="));
      break;
    case LOGB_RELAT_OP_LESSTHAN:      strcpy_P(text_out, (const char *) PSTR("<"));
      break;
    case LOGB_RELAT_OP_GREATERTHAN:   strcpy_P(text_out, (const char *) PSTR(">"));
      break;
    case LOGB_RELAT_OP_LESSTHAN_EQTO: strcpy_P(text_out, (const char *) PSTR("<="));
      break;
    case LOGB_RELAT_OP_GREATTHAN_EQTO: strcpy_P(text_out, (const char *) PSTR(">="));
      break;
    }
    break;

  case LOGB_SYMB_TYPE_ARITHMETIC_OP:
    switch (symbol.val)
    {
    case LOGB_ARITH_OP_ADD: strcpy_P(text_out, (const char *) PSTR("+"));
      break;
    case LOGB_ARITH_OP_SUB: strcpy_P(text_out, (const char *) PSTR("-"));
      break;
    case LOGB_ARITH_OP_MUL: strcpy_P(text_out, (const char *) PSTR("*"));
      break;
    case LOGB_ARITH_OP_DIV: strcpy_P(text_out, (const char *) PSTR("/"));
      break;
    }
    break;

  case LOGB_SYMB_TYPE_ASSOCIATIVITY_OP:
    switch (symbol.val)
    {
    case LOGB_ASSOC_OP_PARANTHESES_LEFT:  strcpy_P(text_out, (const char *) PSTR("("));
      break;
    case LOGB_ASSOC_OP_PARANTHESES_RIGHT: strcpy_P(text_out, (const char *) PSTR(")"));
      break;
    case LOGB_ASSOC_OP_BRACKET_LEFT:      strcpy_P(text_out, (const char *) PSTR("["));
      break;
    case LOGB_ASSOC_OP_BRACKET_RIGHT:     strcpy_P(text_out, (const char *) PSTR("]"));
      break;
    case LOGB_ASSOC_OP_BRACE_LEFT:        strcpy_P(text_out, (const char *) PSTR("{"));
      break;
    case LOGB_ASSOC_OP_BRACE_RIGHT:       strcpy_P(text_out, (const char *) PSTR("}"));
      break;
    }
    break;

  case LOGB_SYMB_TYPE_PUNCTUATION_MARKS_OP:
    switch (symbol.val)
    {
    case LOGB_PUNCT_OP_COMMA:     strcpy_P(text_out, (const char *) PSTR(","));
      break;
    case LOGB_PUNCT_OP_SEMICOLON: strcpy_P(text_out, (const char *) PSTR(";"));
      break;
    case LOGB_PUNCT_OP_COLON:     strcpy_P(text_out, (const char *) PSTR(":"));
      break;
    }
    break;

  case LOGB_SYMB_TYPE_ASSIGNMENT_OP:
    switch (symbol.val)
    {
    case LOGB_ASSIGN_OP_EQUAL:  strcpy_P(text_out, (const char *) PSTR("="));
      break;
    }
    break;


  case LOGB_SYMB_TYPE_LITERALS:
    switch (symbol.val)
    {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    }

    break;

  case LOGB_SYMB_TYPE_VOLTAGE:
    break;

  case LOGB_SYMB_TYPE_CURRENT:
    break;

  case LOGB_SYMB_TYPE_TEMPERATURE:
    break;

  case LOGB_SYMB_TYPE_FLAGS:
    break;

  default:
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////

static inline void mlogb_an_input_fxKey0(void)
{
}
static inline void mlogb_an_input_fxKey1(void)
{
  vscroll.sign_up();
}
static inline void mlogb_an_input_fxKey2(void)
{
  vscroll.sign_down();
}

static inline void mlogb_an_input_fxKey3(void)
{
  class _symbol symbol;
  char text[LOGB_SYMBOL_TEXT_MAX_SIZE];

  symbol.set_type(LOGB_SYMB_TYPE_AN_INPUT);
  symbol.val = vscroll.get_markPosc();
  logb_symbol_add(symbol);

  logb_symbol_get_text(symbol, text);
  //print_text();
}
static inline void mlogb_an_input_fxKey4(void)
{
}

PTRFX_retVOID const mlogb_an_input_fxKey[KB_NUM_KEYS] PROGMEM =
{
  mlogb_an_input_fxKey0, mlogb_an_input_fxKey1, mlogb_an_input_fxKey2, mlogb_an_input_fxKey3, mlogb_an_input_fxKey4
};
void mlogb_an_input_job(void)
{
  char db_an_input[4][LCDAN_STR_MAX_SIZE];
  lcdan_str_lineformat_align_P(&db_an_input[0][0],  PSTR("A1"), 0);
  lcdan_str_lineformat_align_P(&db_an_input[1][0],  PSTR("A2"), 0);
  lcdan_str_lineformat_align_P(&db_an_input[2][0],  PSTR("A3"), 0);
  lcdan_str_lineformat_align_P(&db_an_input[3][0],  PSTR("A4"), 0);

  if (thismenu_sm0 == 0)
  {
    lcd.clear();

    kb_change_keyDo_pgm(mlogb_an_input_fxKey);
    vscroll.init(db_an_input, 2, vscroll_dispShowBuff, 1, FEAT_MARK_ON, 0, 0);
    //sysPrint_lcdPrintPSTR(0,0, PSTR("[SET DATE]"));
    //sysPrint_lcdPrintPSTR(1,STR_CENTER, PSTR("Exit saving date?"));

    thismenu_sm0 = -1;
  }

  vscroll.set_db(db_an_input);
  vscroll.job();

  //USAR:
  vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3); //fil 2 y fil3 are printable
  //o que es MUCHO MEJOR con sentido
  //vscroll_lcd_show_disp(LCD_ROW_2, 2);//cuantas filas se van a imprimir
}
