#ifndef LEDS_H_
#define LEDS_H_

#define NUM_LEDS_MAX 6

#define LED1 34
#define LED2 35
#define LED3 36
#define LED4 37
#define LED5 38
#define LED6 39

extern uint8_t out_leds[NUM_LEDS_MAX];
void out_leds_job(void);
void out_leds_init(void);
#endif
