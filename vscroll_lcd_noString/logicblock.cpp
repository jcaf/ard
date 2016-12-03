#include <Arduino.h>
#include "system.h"

#include "logicblock.h"

//////////////////////////////////////////////////////////////////////////////////////
uint8_t digital_input_readvalDI1(void)
{
    return 1;   //digitalRead(DIG_PIN_DI1);
}
uint8_t digital_input_readvalDI2(void)
{
    return 1;   //digitalRead(DIG_PIN_DI2);
}
uint8_t digital_input_readvalDI3(void)
{
    return 1;   //digitalRead(DIG_PIN_DI3);
}
uint8_t digital_input_readvalDI4(void)
{
    return 0;//digitalRead(DIG_PIN_DI4);
}


void _digital_input::set_addr_fx_read_pin(PTRFX_retUINT8_T addr_read_pin)
{
    fx_read_pin = addr_read_pin;
}

void _digital_input::set_logic(uint8_t l)
{
    logic = l;
}

uint8_t _digital_input::get_logic(void)
{
    return logic;
}

uint8_t _digital_input::get_pinvalue_internal(void)
{
    return fx_read_pin();
}

uint8_t _digital_input::get_pinvalue_withlogic(void)
{
    return (logic == 1)? fx_read_pin(): !fx_read_pin();
}

class _digital_input digital_input[4];

//////////////////////////////////////////////////////////////////////////////////////

//return the value of internal ADC (direct)
uint16_t analog_input_readvalA1(void)
{
    return (uint16_t) (1024/1.0f);//analogRead(AN_PIN_A3);
}
uint16_t analog_input_readvalA2(void)
{
    return (uint16_t) (1024/2.0f);//analogRead(AN_PIN_A2);
}
uint16_t analog_input_readvalA3(void)
{
    return (uint16_t) (1024/3.0f);//analogRead(AN_PIN_A1);
}
uint16_t analog_input_readvalA4(void)
{
    return (uint16_t) (1024/4.0f);//analogRead(AN_PIN_A0);
}

void _analog_input::set_addr_fx_read_pin(PTRFX_retUINT16_T addr_fx_read_pin)
{
    fx_read_pin = addr_fx_read_pin;
}

void _analog_input::set_scale_factor(double sf)
{
    scale_factor = sf;
}
void _analog_input::set_offset(double o)
{
    offset = o;
}
void _analog_input::set_units(uint8_t u)
{
    offset = u;
}
double _analog_input::get_scale_factor(void)
{
    return scale_factor;
}
double _analog_input::get_offset(void)
{
    return offset;
}
uint8_t _analog_input::get_units(void)
{
    return units;
}

uint16_t _analog_input::get_adcvalue_internal(void)
{
    return fx_read_pin();
}

double _analog_input::get_adcvalue_calibrated(void)
{
    return (fx_read_pin() * scale_factor) + offset;
}


class _analog_input analog_input[4];

//////////////////////////////////////////////////////////////////////////////////////
uint8_t rele_1_readvalue(void)
{
    //return ReadPin(PORTRxRELE1, PINxRELE1);
    return 1;
}
uint8_t rele_2_readvalue(void)
{
    //return ReadPin(PORTRxRELE2, PINxRELE2);
    return 1;
}
uint8_t rele_3_readvalue(void)
{
    //return ReadPin(PORTRxRELE3, PINxRELE3);
    return 1;
}
uint8_t rele_4_readvalue(void)
{
    //return ReadPin(PORTRxRELE4, PINxRELE4);
    return 1;
}
uint8_t rele_5_readvalue(void)
{
    //return ReadPin(PORTRxRELE5, PINxRELE5);
    return 1;
}
//
void rele_1_outvalue(uint8_t val)
{
    //(val == 1)? PinTo1(PORTRxRELE1, PINxRELE1): PinTo0(PORTRxRELE1, PINxRELE1);
    printf("%u \n", (val == 1)? 1: 0);
}
void rele_2_outvalue(uint8_t val)
{
    //(val == 1)? PinTo1(PORTRxRELE2, PINxRELE2): PinTo0(PORTRxRELE2, PINxRELE2);
    printf("%u \n", (val == 1)? 1: 0);
}
void rele_3_outvalue(uint8_t val)
{
    //(val == 1)? PinTo1(PORTRxRELE3, PINxRELE3): PinTo0(PORTRxRELE3, PINxRELE3);
    printf("%u \n", (val == 1)? 1: 0);
}
void rele_4_outvalue(uint8_t val)
{
    //(val == 1)? PinTo1(PORTRxRELE4, PINxRELE4): PinTo0(PORTRxRELE4, PINxRELE4);
    printf("%u \n", (val == 1)? 1: 0);

}
void rele_5_outvalue(uint8_t val)
{
    //(val == 1)? PinTo1(PORTRxRELE5, PINxRELE5): PinTo0(PORTRxRELE5, PINxRELE5);
    printf("%u \n", (val == 1)? 1: 0);
}


void _rele::set_addr_fx_write_pin(PTRFX_retVOID_arg1_UINT8_T addr_fx_write_pin)
{
    fx_write_pin = addr_fx_write_pin;
}
void _rele::set_value(uint8_t value)
{
    fx_write_pin(value);
}

void _rele::set_addr_fx_read_pin(PTRFX_retUINT8_T addr_fx_read_pin)
{
    fx_read_pin = addr_fx_read_pin;
}
uint8_t _rele::get_pinvalue(void)
{
    return fx_read_pin();
}

class _rele rele[5];

//////////////////////////////////////////////////////////////////////////////////////
void _symbol::set_type(int8_t typ)
{
    type  = typ;
}
int8_t _symbol::get_type(void) const
{
    return type;
}


class _symbol logb_symbol[10];
//////////////////////////////////////////////////////////////////////////////////////


bool logb_logicOP(uint8_t logicOP, bool lvalue, bool rvalue)
{
    if (logicOP == LOGB_LOGICOP_AND)
        return (lvalue && rvalue);
    else if (logicOP == LOGB_LOGICOP_OR)
        return (lvalue || rvalue);
}

/*
int main()
{
    size_t logicOP;
    bool lvalue, rvalue, result;

int8_t operand;

    digital_input[0].set_logic(LOGB_DIGINPUT_POSITIVE_LOGIC);
    digital_input[1].set_logic(LOGB_DIGINPUT_POSITIVE_LOGIC);
    digital_input[2].set_logic(LOGB_DIGINPUT_POSITIVE_LOGIC);
    digital_input[3].set_logic(LOGB_DIGINPUT_POSITIVE_LOGIC);
    digital_input[0].set_addr_fx_read_pin(digital_input_readvalDI1);
    digital_input[1].set_addr_fx_read_pin(digital_input_readvalDI2);
    digital_input[2].set_addr_fx_read_pin(digital_input_readvalDI3);
    digital_input[3].set_addr_fx_read_pin(digital_input_readvalDI4);


    //necesito el valor de salida "calibrado"
    analog_input[0].set_scale_factor(1);//*
    analog_input[0].set_offset(0);//+
    analog_input[0].set_units(LOGB_ANINPUT_UNITS_ADIMENSIONAL);
    analog_input[1].set_scale_factor(1);//*
    analog_input[1].set_offset(0);//+
    analog_input[1].set_units(LOGB_ANINPUT_UNITS_ADIMENSIONAL);
    analog_input[2].set_scale_factor(1);//*
    analog_input[2].set_offset(0);//+
    analog_input[2].set_units(LOGB_ANINPUT_UNITS_ADIMENSIONAL);
    analog_input[3].set_scale_factor(5/1024.0f);//*
    analog_input[3].set_offset(0);//+
    analog_input[3].set_units(LOGB_ANINPUT_UNITS_ADIMENSIONAL);

    analog_input[0].set_addr_fx_read_pin(analog_input_readvalA1);
    analog_input[1].set_addr_fx_read_pin(analog_input_readvalA2);
    analog_input[2].set_addr_fx_read_pin(analog_input_readvalA3);
    analog_input[3].set_addr_fx_read_pin(analog_input_readvalA4);

    rele[0].set_addr_fx_write_pin(rele_1_outvalue);


    //Simulating
    /////////////////////////////////////////////////////////////////////
    //1.- set by menu

    // dig0 && dig1
    operand = 0;
    logb_symbol[0].set_type(LOGB_SYMB_TYPE_DIG_INPUT);
    logb_symbol[0].pval = (class _digital_input *) &digital_input[operand];

    operand = 0;
    logb_symbol[1].set_type(LOGB_SYMB_TYPE_LOGICAL_OP);
    logb_symbol[1].val = LOGB_LOGICOP_AND;

    operand = 1;
    logb_symbol[2].set_type(LOGB_SYMB_TYPE_DIG_INPUT);
    logb_symbol[2].pval = (class _digital_input *) &digital_input[operand];

    // an0 > an1
    operand = 3;
    logb_symbol[3].set_type(LOGB_SYMB_TYPE_AN_INPUT);
    logb_symbol[3].pval = (class _analog_input *) &analog_input[operand];

    //operand = 0;
    logb_symbol[4].set_type(LOGB_SYMB_TYPE_RELATIONAL_OP);//>
    logb_symbol[4].val = LOGB_RELAT_OP_GREATERTHAN;

    operand = 1;
    logb_symbol[5].set_type(LOGB_SYMB_TYPE_AN_INPUT);
    logb_symbol[5].pval = (class _analog_input *) &analog_input[operand];

    /////////////////////////////////////////////////////////////////////
    //2.- read valid

    uint8_t i;

    //example 1
    i = 0;
    if (logb_symbol[i].get_type() == LOGB_SYMB_TYPE_DIG_INPUT)
        lvalue = ((class _digital_input *)logb_symbol[i].pval)->get_pinvalue_withlogic();

    i = 1;
    if (logb_symbol[i].get_type() == LOGB_SYMB_TYPE_LOGICAL_OP)
        logicOP = logb_symbol[1].val;

    i = 2;
    if (logb_symbol[i].get_type() == LOGB_SYMB_TYPE_DIG_INPUT)
        rvalue = ((class _digital_input *)logb_symbol[i].pval)->get_pinvalue_withlogic();

    result  = logb_logicOP(logicOP, lvalue, rvalue);
    printf("%u \n", result);

    //example 2
    double lvalue_f, rvalue_f;
    i = 3;
    if (logb_symbol[i].get_type() == LOGB_SYMB_TYPE_AN_INPUT)
        lvalue_f = ((class _analog_input *)logb_symbol[i].pval)->get_adcvalue_calibrated();

    i = 4;
    if (logb_symbol[i].get_type() == LOGB_SYMB_TYPE_RELATIONAL_OP)
        logicOP = logb_symbol[i].val;

    i = 5;
    if (logb_symbol[i].get_type() == LOGB_SYMB_TYPE_AN_INPUT)
        rvalue_f = ((class _analog_input *)logb_symbol[i].pval)->get_adcvalue_calibrated();

    //result  = logb_logicOP(logicOP, lvalue, rvalue);

    if (logicOP == LOGB_RELAT_OP_GREATERTHAN)
        result  = lvalue_f >= (5/4.0f);

    printf("%.2f %.2f %u \n", lvalue_f, rvalue_f, result);

    //    printf("valor a rele1: \n");
    //    rele_1_outvalue(0);

    return 0;
}

*/

///////////////////////////////////////////////////
//    for (uint8_t i=0; i<1 ; i++)
//    {
//        if (logb_symbol[i].get_type() == LOGB_SYMB_TYPE_DIG_INPUT)
//        {
//            r = ((class _relay *)logb_symbol[0].p)[0].readval();
//        }
//    }



