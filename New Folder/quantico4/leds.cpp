#include <Arduino.h>
#include "leds.h"
#include "system.h"

/*
uint8_t out_leds[NUM_LEDS_MAX];

#define LED1 34
#define LED2 35
#define LED3 36
#define LED4 37
#define LED5 38
#define LED6 39

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

 void out_leds_job(void)
 {
     digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,HIGH);
    digitalWrite(LED4,HIGH);
    digitalWrite(LED5,HIGH);
    digitalWrite(LED6,HIGH);

 	out_leds_read();
 }


*/
