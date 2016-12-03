#include <Arduino.h>
#include <EEPROM.h>
class _a
{
  public:
  uint8_t v[10];
}a;
#define EE_NUMMAX 5
struct _ee
{
  uint8_t v[10];
  uint8_t c[5];
  class _a a[5];
  
}ee[EE_NUMMAX];

#define ee_size sizeof(ee)/sizeof(ee[0])


void setup() 
{
  // put your setup code here, to run once:
  uint16_t acc = 0;
  
  ee[0].a[0].v[0] = 1;
  ee[0].a[1].v[1] = 11;
  
  for (uint8_t i=0; i<EE_NUMMAX; i++)
  {
    EEPROM.put(acc,&ee[i]);
    acc +=ee_size;
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
