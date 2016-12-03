#include <Arduino.h>
#include "leds.h"
#include "system.h"


uint8_t out_leds[NUM_LEDS_MAX];


/*
static inline uint8_t out_leds_readLED1(void)
{
    return digitalRead(LED1);
}
static inline uint8_t out_leds_readLED2(void)
{
    return digitalRead(LED2);
}
static inline uint8_t out_leds_readLED3(void)
{
    return digitalRead(LED3);
}
static inline uint8_t out_leds_readLED4(void)
{
    return digitalRead(LED4);
}
static inline uint8_t out_leds_readLED5(void)
{
    return digitalRead(LED5);
}
static inline uint8_t out_leds_readLED6(void)
{
    return digitalRead(LED6);
}

struct _outleds
{
	PTRFX_retUINT8_T read;

}outLeds[NUM_LEDS_MAX]=
{
	out_leds_readLED1,
	out_leds_readLED2,
	out_leds_readLED3,
	out_leds_readLED4,
	out_leds_readLED5,
	out_leds_readLED6,
};



static void out_leds_read(void)
{
	for (uint8_t i=0; i<NUM_LEDS_MAX; i++ )
	out_leds[i] = outLeds[i].read();

}
*/
 void out_leds_init(void)
 {
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,LOW);
    digitalWrite(LED3,LOW);
    digitalWrite(LED4,LOW);
    digitalWrite(LED5,LOW);
    digitalWrite(LED6,LOW);

    pinMode(LED1,OUTPUT);
    pinMode(LED2,OUTPUT);
    pinMode(LED3,OUTPUT);
    pinMode(LED4,OUTPUT);
    pinMode(LED5,OUTPUT);
    pinMode(LED6,OUTPUT);

 }

 void out_leds_job(void)
 {
// 	out_leds_read();
 }


