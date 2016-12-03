#include <Arduino.h>
#include "rele.h"
#include "system.h"

uint8_t do_rele[NUM_RELE_MAX];

#define RELE1 29
#define RELE2 30
#define RELE3 31
#define RELE4 32
#define RELE5 33
/*
uint8_t do_rele_readRELE1(void)
{
    return digitalRead(RELE1);
}
uint8_t do_rele_readRELE2(void)
{
    return digitalRead(RELE2);
}
uint8_t do_rele_readRELE3(void)
{
    return digitalRead(RELE3);
}
uint8_t do_rele_readRELE4(void)
{
    return digitalRead(RELE4);
}
uint8_t do_rele_readRELE5(void)
{
    return digitalRead(RELE5);
}

struct _doRele
{
	PTRFX_retUINT8_T read;

}doRele[NUM_RELE_MAX]=
{
	do_rele_readRELE1,
	do_rele_readRELE2,
	do_rele_readRELE3,
	do_rele_readRELE4,
	do_rele_readRELE5,
};

void do_rele_read(void)
{
	for (uint8_t i=0; i<NUM_RELE_MAX; i++ )
		do_rele[i] = doRele[i].read();
}
*/
void rele_job(void)
{
	//do_rele_read();
}

void rele_init(void)
{
    digitalWrite(RELE1,LOW);
    digitalWrite(RELE2,LOW);
    digitalWrite(RELE3,LOW);
    digitalWrite(RELE4,LOW);
    digitalWrite(RELE5,LOW);

    pinMode(RELE1,OUTPUT);
    pinMode(RELE2,OUTPUT);
    pinMode(RELE3,OUTPUT);
    pinMode(RELE4,OUTPUT);
    pinMode(RELE5,OUTPUT);
}

