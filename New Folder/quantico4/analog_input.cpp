#include <Arduino.h>
#include "analog_input.h"
#include "system.h"
/*
#define AN_PIN_A0 A0
#define AN_PIN_A1 A1
#define AN_PIN_A2 A2
#define AN_PIN_A3 A3

uint8_t analog_input[NUM_ANALOG_INPUT_MAX];

 //ANALOG CONECTOR -> A1(A0)->A4(A3) las entradas estan al reves A4(A3)->A1(A0)

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

struct _analogInput
{
	PTRFX_retUINT16_T read;

}analogInput[NUM_ANALOG_INPUT_MAX]=
{
	analog_input_readA1,
	analog_input_readA2,
	analog_input_readA3,
	analog_input_readA4
};

 void analog_input_read(void)
 {
 	for (uint8_t i=0; i<NUM_ANALOG_INPUT_MAX; i++ )
		analog_input[i] = analogInput[i].read();

 }

 void analog_input_job(void)
 {
 	analog_input_read();
 }
*/

