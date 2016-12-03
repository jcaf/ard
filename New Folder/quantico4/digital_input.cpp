#include <Arduino.h>
#include "digital_input.h"
#include "system.h"

uint8_t digital_input[NUM_DIG_INPUT_MAX];

#define DIG_PIN_DI1 40
#define DIG_PIN_DI2 41
#define DIG_PIN_DI3 42
#define DIG_PIN_DI4 43


inline static uint8_t digital_input_readDI1(void)
{
    return digitalRead(DIG_PIN_DI1);
}
inline static uint8_t digital_input_readDI2(void)
{
    return digitalRead(DIG_PIN_DI2);
}
inline static uint8_t digital_input_readDI3(void)
{
    return digitalRead(DIG_PIN_DI3);
}
inline static uint8_t digital_input_readDI4(void)
{
    return digitalRead(DIG_PIN_DI4);
}

struct _digitalInput
{
	PTRFX_retUINT8_T read;

}digitalInput[NUM_DIG_INPUT_MAX]=
{
	digital_input_readDI1,
	digital_input_readDI2,
	digital_input_readDI3,
	digital_input_readDI4
};

void digital_input_read(void)
{
	for (uint8_t i=0; i<NUM_DIG_INPUT_MAX; i++ )
	digital_input[i] = digitalInput[i].read();

}

 void digital_input_job(void)
 {
 	digital_input_read();
 }



