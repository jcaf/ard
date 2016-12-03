#include <Arduino.h>
#include "analog_input.h"
#include "system.h"

#define AN_PIN_A0 A0
#define AN_PIN_A1 A1
#define AN_PIN_A2 A2
#define AN_PIN_A3 A3

float analog_input[NUM_ANALOG_INPUT_MAX];

//ANALOG CONECTOR -> A1(A0)->A4(A3) las entradas estan al reves A4(A3)->A1(A0)
/*
inline static uint16_t analog_input_readA1(void)
{
    return analogRead(AN_PIN_A3);
}
inline static uint16_t analog_input_readA2(void)
{
    return analogRead(AN_PIN_A2);
}
inline static uint16_t analog_input_readA3(void)
{
    return analogRead(AN_PIN_A1);
}
inline static uint16_t analog_input_readA4(void)
{
    return analogRead(AN_PIN_A0);
}

const struct _analogInput
{
    PTRFX_retUINT16_T read;

} analogInput[NUM_ANALOG_INPUT_MAX] PROGMEM=
{
    analog_input_readA1,
    analog_input_readA2,
    analog_input_readA3,
    analog_input_readA4
};

void analog_input_read(void)
{
    PTRFX_retUINT16_T f;
    for (uint8_t i=0; i<NUM_ANALOG_INPUT_MAX; i++ )
    {
        //		analog_input[i] = (analogInput[i].read()*5)/1023.0;
        f = (PTRFX_retUINT16_T)pgm_read_word(&analogInput[i].read);
        analog_input[i] = (f()*5)/1023.0 ;
    }
}
*/
void analog_input_job(void)
{
    //analog_input_read();
    analog_input[0] = analogRead(AN_PIN_A3)*5/1023.0;
    analog_input[1] = analogRead(AN_PIN_A2)*5/1023.0;
    analog_input[2] = analogRead(AN_PIN_A1)*5/1023.0;
    analog_input[3] = analogRead(AN_PIN_A0)*5/1023.0;
}


