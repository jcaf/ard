#include <Arduino.h>
#include "rele.h"
#include "system.h"
/*
uint8_t do_rele[NUM_RELE_MAX];

#define RELE1 29
#define RELE2 30
#define RELE3 31
#define RELE4 32
#define RELE5 33

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

void rele_job(void)
{

    digitalWrite(RELE1,HIGH);
    digitalWrite(RELE2,HIGH);
    digitalWrite(RELE3,HIGH);
    digitalWrite(RELE4,HIGH);
    digitalWrite(RELE5,HIGH);


	do_rele_read();
}


*/
