#ifndef LOGICBLOCK_H_
#define LOGICBLOCK_H_

class _digital_input
{
    uint8_t logic;
    PTRFX_retUINT8_T fx_read_pin;
public:
    void set_addr_fx_read_pin(PTRFX_retUINT8_T addr_read_pin);
    void set_logic(uint8_t l);
    uint8_t get_logic(void);
    uint8_t get_pinvalue_internal(void);
    uint8_t get_pinvalue_withlogic(void);
};


class _analog_input
{
    double scale_factor;
    double offset;
    uint8_t units;
    PTRFX_retUINT16_T fx_read_pin;
public:
    void set_addr_fx_read_pin(PTRFX_retUINT16_T addr_fx_read_pin);
    void set_scale_factor(double sf);
    void set_offset(double o);
    void set_units(uint8_t u);
    double get_scale_factor(void);
    double get_offset(void);
    uint8_t get_units(void);
    uint16_t get_adcvalue_internal(void);
    double get_adcvalue_calibrated(void);
};

class _rele
{
    unsigned enable:1;
    unsigned busy:1;
    unsigned __a:6;

    PTRFX_retVOID_arg1_UINT8_T fx_write_pin;
    PTRFX_retUINT8_T fx_read_pin;
public:
    void set_addr_fx_write_pin(PTRFX_retVOID_arg1_UINT8_T addr_fx_write_pin);
    void set_value(uint8_t value);
    void set_addr_fx_read_pin(PTRFX_retUINT8_T addr_fx_read_pin);
    uint8_t get_pinvalue(void);
};

class _symbol
{
private:
    int8_t type;//end of expression
    //char c;//printable char

public:
    size_t val;//by val
    void *pval;//by ref

    void set_type(int8_t typ);
    int8_t get_type(void) const;
};

//////////////////////////////////////////////////////////////////////
enum
{
    LOGB_SYMB_TYPE_DIG_INPUT,
    LOGB_SYMB_TYPE_AN_INPUT,
    LOGB_SYMB_TYPE_RELAY,
    //
    LOGB_SYMB_TYPE_LOGICAL_OP,
    LOGB_SYMB_TYPE_RELATIONAL_OP,
    LOGB_SYMB_TYPE_ARITHMETIC_OP,
    LOGB_SYMB_TYPE_ASSOCIATIVITY_OP,
    LOGB_SYMB_TYPE_PUNCTUATION_MARKS_OP,
    LOGB_SYMB_TYPE_ASSIGNMENT_OP,

    //
    LOGB_SYMB_TYPE_LITERALS,
    LOGB_SYMB_TYPE_VOLTAGE,
    LOGB_SYMB_TYPE_CURRENT,
    LOGB_SYMB_TYPE_TEMPERATURE,
    LOGB_SYMB_TYPE_FLAGS,
};



enum _LOGICAL_OP
{
    LOGB_LOGICOP_AND,
    LOGB_LOGICOP_OR,
    LOGB_LOGICOP_NOT,
};

enum _RELATIONAL_OP
{
    LOGB_RELAT_OP_EQUAL_TO,
    LOGB_RELAT_OP_NOTEQUAL_TO,
    LOGB_RELAT_OP_LESSTHAN,
    LOGB_RELAT_OP_GREATERTHAN,
    LOGB_RELAT_OP_LESSTHAN_EQTO,
    LOGB_RELAT_OP_GREATTHAN_EQTO,
};
enum _ARITHMETIC_OP
{
    LOGB_ARITH_OP_ADD,
    LOGB_ARITH_OP_SUB,
    LOGB_ARITH_OP_MUL,
    LOGB_ARITH_OP_DIV,
};

enum _ASSOCIATIVITY_OP  //bracket logb_symbols
{
    LOGB_ASSOC_OP_PARANTHESES_LEFT,//(
    LOGB_ASSOC_OP_PARANTHESES_RIGHT,//)
    LOGB_ASSOC_OP_BRACKET_LEFT,//[
    LOGB_ASSOC_OP_BRACKET_RIGHT,//]
    LOGB_ASSOC_OP_BRACE_LEFT,//{
    LOGB_ASSOC_OP_BRACE_RIGHT,//}
};

enum _PUNCTUATION_MARKS_OP
{
    LOGB_PUNCT_OP_COMMA,// ,
    LOGB_PUNCT_OP_SEMICOLON,// ;
    LOGB_PUNCT_OP_COLON,// :
};

enum _ASSIGNMENT_OP
{
    LOGB_ASSIGN_OP_EQUAL,// =
};

///////////////////////////////////////////////////////////
enum
{
    LOGB_DIGINPUT_NEGATIVE_LOGIC,
    LOGB_DIGINPUT_POSITIVE_LOGIC,
};
enum
{
    LOGB_ANINPUT_UNITS_ADIMENSIONAL,
};



extern class _symbol logb_symbol[10];

#define LOGB_SYMBOL_TEXT_MAX_SIZE 10
#endif // LOGICBLOCK_H_
