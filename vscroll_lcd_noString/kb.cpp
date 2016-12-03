#include <Arduino.h>

#include "ikey.h"
#include "lcd_bklight.h"
#include "kb.h"

#include "system.h"
//#include "leds.h"

void kb_processKeyRead_D(void)
{
    if (kb_get_ready2read(0))
    {
//TogglingBit(PORTWxLED6,PINxLED6);
        kb_clear_ready2read(0);
        kb_execfunct(0);
    }
    if (kb_get_ready2read(1))
    {
//TogglingBit(PORTWxLED6,PINxLED6);
        kb_clear_ready2read(1);
        kb_execfunct(1);
    }
    if (kb_get_ready2read(2))
    {
//TogglingBit(PORTWxLED6,PINxLED6);
        kb_clear_ready2read(2);
        kb_execfunct(2);
    }
    if (kb_get_ready2read(3))
    {
//TogglingBit(PORTWxLED6,PINxLED6);
        kb_clear_ready2read(3);
        kb_execfunct(3);
    }
    if (kb_get_ready2read(4))
    {
//TogglingBit(PORTWxLED6,PINxLED6);
        kb_clear_ready2read(4);
        kb_execfunct(4);
    }
}
//ISR
void kb_job(void)
{

    key_scan();


     if (lcd_bklight_is_ON())
     {
         if (!AllKeysCompletedProcessing())
         {
             lcd_bklight_timming_reset();
         }
         key_parsing();
     }

}

//#define FREQ_SYSTEM 16000000 //->def in system.h
#define CTC_SET_OCRnA(CTC_FREQ, CTC_PRESCALER) ( (uint8_t)( (FREQ_SYSTEM/ (2*CTC_PRESCALER*CTC_FREQ)) -1) )

void InitTCNT1(void)
{
    //cli(); //arduino llama a sei() antes de entrar
    //Arduino (wiring.c) usa el timer1:
    // put timer 1 in 8-bit phase correct pwm mode
    //#if defined(TCCR1A) && defined(WGM10)
        //sbi(TCCR1A, WGM10);
    //#endif
    //
    //si despues aparece el  OCR1A, y luego se limpia el TCCR1A, no funciona
    //correctamente el hardware, es mejor que OCR1A se escriba despues de todo
    TCCR1A=0;
    TCCR1B = (1 << WGM12) | (1 << CS12) |(0 << CS11)|(1 << CS10);//CTC PRES=1024
    TCNT1 = 0x0000;//THE GRAND FIX! this was causing a delay to respond the keys because t= 1/(16e6/1024) -> t*(65535-77) = 4.18s
                    //if the TCNT1 > set OCR1A(77) -> TCNT1 count to 0xFFFF and then, catch the first interrupt @ 5ms
    OCR1A = CTC_SET_OCRnA( 100,1024L);//77 @5ms->freq 100hz

    //OCR1A = CTC_SET_OCRnA( 25,1024L);//@20ms->freq 25hz
    //TIMSK1 |= (1<<OCIE1A);
}

void kb_change_keyDo_pgm(PTRFX_retVOID const *  keyDo)
{
    uint8_t i;
    for (i=0; i<KB_NUM_KEYS; i++)
        key[i].keyDo = (PTRFX_retVOID)pgm_read_word(&keyDo[i]);
}
/*
void kb_change_keyDo(PTRFX_retVOID *keyDo)
{
    uint8_t i;
    for (i=0; i<KB_NUM_KEYS; i++)
        key[i].keyDo = keyDo[i];
}
*/


