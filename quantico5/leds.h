#ifndef LEDS_H_
#define LEDS_H_

#define NUM_LEDS_MAX 6

//Arduino MEGA pinout
#define LED1 34 //PC3
#define LED2 35 //PC2
#define LED3 36 //PC1
#define LED4 37 //PC0
#define LED5 38 //PD7
#define LED6 39 //PG2


//ATmega2560 pinout
#define DDRxLED1	DDRC
#define PORTWxLED1	PORTC
#define PORTRxLED1	PINC
#define PINxLED1	3

#define DDRxLED2	DDRC
#define PORTWxLED2	PORTC
#define PORTRxLED2	PINC
#define PINxLED2	2

#define DDRxLED3	DDRC
#define PORTWxLED3	PORTC
#define PORTRxLED3	PINC
#define PINxLED3	1

#define DDRxLED4	DDRC
#define PORTWxLED4	PORTC
#define PORTRxLED4	PINC
#define PINxLED4	0

#define DDRxLED5	DDRD
#define PORTWxLED5	PORTD
#define PORTRxLED5  PIND
#define PINxLED5	7

#define DDRxLED6	DDRG
#define PORTWxLED6	PORTG
#define PORTRxLED6	PING
#define PINxLED6	2



extern uint8_t out_leds[NUM_LEDS_MAX];
void out_leds_job(void);
void out_leds_init(void);
#endif

