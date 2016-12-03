#include <avr/pgmspace.h>
    
    typedef uint8_t (*PTRFX_retUINT8_T)(void);
    typedef void (*PTRFX_retVOID)(void);
void a()
{
  Serial.println("a");
  }
void b()
{
  Serial.println("b");
  }
void c()
{
  Serial.println("c");
  }
void d()
{
  Serial.println("d");
  }
PTRFX_retVOID const pd PROGMEM = d;
 PTRFX_retVOID  const m[8][5] PROGMEM 
= {
  {a,b},
  {0},
  {0},
  {0},
  {0},
  {0},
  {0},
  {c,d}
};

PTRFX_retVOID p;

void setup() 
{
  // put your setup code here, to run once:
Serial.begin(230400);
//p= &m[]
//p =  (PTRFX_retVOID)pgm_read_word(&pd);
p =  (PTRFX_retVOID)pgm_read_word(&m[7][0]);
p();
//m[0][1]();
//m[7][0]();

}

void loop() {
  // put your main code here, to run repeatedly:

}
