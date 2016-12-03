/* ESTE CODIGO ESTA TOTALMENTE PURGADO,,, 
 * LAS RESISTENCIAS DE TERMINACION SON DE 100 OHM
 * LA DISTANCIA ENTRE EL MASTER Y ESCLAVO ES DE 10 METROS
 * TODOS LOS TRAFO DE PULSO 78613/2JC
 */
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include "system.h"
#include "LTC68042.h"
#include "ltc6804.h"

void SPI_deselectAllperipherals(void)
{
    digitalWrite(WIZNET_CS, HIGH);
    digitalWrite(SDCARD_CS, HIGH);
    digitalWrite(LTC6804_CS, HIGH);
}
void LTC6804_initialize()
{
    DDRE=1<<PE3;    //PE3 = LTC_CS
	  DDRG = 1<<PG5;  //PG5 = SDCARD_CS
    PORTG = 1<<PG5;

    DDRB = (1<<PB4) | (1<<PB2)|(1<<PB1) | (1<<PB0); //PB4 =WIZNET_CS
    //LTC6804_SPI_SPEED 500E3 -> // (1<<SPR1) | (0<<SPR0) = fosc/32 with SPI2X = 1
    SPSR=1<<SPI2X;
    SPCR = (1<<SPE)|(1<<MSTR)| (1<<CPOL) | (1<<CPHA) | (1<<SPR1) | (1<<SPR0);//configurar SPI modo 3

    PORTB=(1<<4) | (1<<0); //CS WIZNET DESHABLITIADO, Y MANTENER COMO SALIDA CS x SPI-HARDWARE, //SINO EL HARDWARE LO CAMBIA A ESCLAVO
}

/*
 * 2 opciones de config. SPI:
 * 1)LTC6804_initialize() config inputs/output pins, freq. modo
 * o
 * 2) Bloque SPI CONFIG BEGIN-END
 */

void setup() 
{
  LTC6804_initialize();


  //SPI CONFIG BEGIN
	/*
	SPI_deselectAllperipherals();
	pinMode(WIZNET_CS, OUTPUT);
	pinMode(SDCARD_CS, OUTPUT);
	pinMode(LTC6804_CS, OUTPUT);
	ltc6804_SPI_access();
	SPI.begin();
  //SPI CONFIG END
	*/
	Serial.begin(230400);
	Serial.println("corriendo");
}

void loop() 
{
  ltc6804_job();
}
