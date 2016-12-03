#include <Arduino.h>
  typedef uint8_t (*PTRFX_retUINT8_T)(void);
    typedef uint16_t (*PTRFX_retUINT16_T)(void);
    typedef void (*PTRFX_retVOID)(void);
#define NUM_ANALOG_INPUT_MAX 4
#define AN_PIN_A0 A0
#define AN_PIN_A1 A1
#define AN_PIN_A2 A2
#define AN_PIN_A3 A3

float analog_input[NUM_ANALOG_INPUT_MAX];

 //ANALOG CONECTOR -> A1(A0)->A4(A3) las entradas estan al reves A4(A3)->A1(A0)

inline static uint16_t analog_input_readA1(void)
{
  Serial.println(1);
    return analogRead(AN_PIN_A3);
}
inline static uint16_t analog_input_readA2(void)
{
  Serial.println(2);
    return analogRead(AN_PIN_A2);
}
inline static uint16_t analog_input_readA3(void)
{
  Serial.println(3);
    return analogRead(AN_PIN_A1);
}
inline static uint16_t analog_input_readA4(void)
{
  Serial.println(4);
    return analogRead(AN_PIN_A0);
}

const 
struct _analogInput
{
  PTRFX_retUINT16_T read;

}analogInput[NUM_ANALOG_INPUT_MAX] PROGMEM=
{
  analog_input_readA1,
  analog_input_readA2,
  analog_input_readA3,
  analog_input_readA4
};

 void analog_input_read(void)
 {
  for (uint8_t i=0; i<NUM_ANALOG_INPUT_MAX; i++ )
    //analog_input[i] = (analogInput[i].read()*5)/1023.0;
((PTRFX_retVOID)pgm_read_word(&analogInput[i].read))();
 }
 
 
// void kb_change_keyDo_pgm(PTRFX_retVOID const *  keyDo)
//{
//    uint8_t i;
//    for (i=0; i<KB_NUM_KEYS; i++)
//        key[i].keyDo = (PTRFX_retVOID)pgm_read_word(&analogInput[i]);
//}

 
void setup() {
  // put your setup code here, to run once:
Serial.begin(230400);
analog_input_read();
}

void loop() {
  // put your main code here, to run repeatedly:

}
