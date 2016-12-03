#include <Arduino.h>
#include "digital_input.h"
#include "system.h"

uint8_t digital_input[NUM_DIG_INPUT_MAX];

#define DIG_PIN_DI1 40
#define DIG_PIN_DI2 41
#define DIG_PIN_DI3 42
#define DIG_PIN_DI4 43

/*
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

const struct _digitalInput
{
    PTRFX_retUINT8_T read;

} digitalInput[NUM_DIG_INPUT_MAX] PROGMEM =
{
    digital_input_readDI1,
    digital_input_readDI2,
    digital_input_readDI3,
    digital_input_readDI4
};

void digital_input_read(void)
{
    PTRFX_retUINT8_T f;
    for (uint8_t i=0; i<NUM_DIG_INPUT_MAX; i++ )
    {
        //digital_input[i] = digitalInput[i].read();
        f = (PTRFX_retUINT8_T)pgm_read_word(&digitalInput[i].read);
        digital_input[i] = f();
    }
}
*/
void digital_input_job(void)
{
    //digital_input_read();
    digital_input[0]= digitalRead(DIG_PIN_DI1);
    digital_input[1]= digitalRead(DIG_PIN_DI2);
    digital_input[2]= digitalRead(DIG_PIN_DI3);
    digital_input[3]= digitalRead(DIG_PIN_DI4);
}

void digital_input_init(void)
{
    pinMode(DIG_PIN_DI1, INPUT);
    pinMode(DIG_PIN_DI2, INPUT);
    pinMode(DIG_PIN_DI3, INPUT);
    pinMode(DIG_PIN_DI4, INPUT);
}



