#include "morepgmspace.h"
const uint16_t PROGMEM v[10] =    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
uint16_t a, i;

void setup() 
{
Serial.begin(230400);

// a = pgm_read_word(&v[i]);
for (i=0; i<10; i++)
{
  a = pgm_read_word_far(GET_FAR_ADDRESS(v[0]) + i * sizeof(v[0]));
Serial.println(a);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
