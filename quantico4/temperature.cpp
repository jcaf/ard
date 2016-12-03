#include <Arduino.h>
#include "temperature.h"

/////////////////////////////////////////////////////////////////////////////////////
//VISHAY NTCLE413E2103F102L 1%
//- 40 C to + 105 C
/////////////////////////////////////////////////////////////////////////////////////
#define RESISTOR_UP 9.960E3//fluke 175
#define NTC_NOMINAL 10.0E3//+-1%

#define ST_A 0.00335401643468053
#define ST_B 0.000300130825115663
#define ST_C 5.08516494379094E-06
#define ST_D 2.18765049258341E-07
//Steinhart equation
float ntc10k_st(uint16_t ADC_CURRENT_VAL, uint16_t ADC_TOP_VAL)
{
  float Rm=RESISTOR_UP/(( ((float)ADC_TOP_VAL)/ADC_CURRENT_VAL) -1);
  //float Rm = RESISTOR_UP/((29890/ADC_CURRENT_VAL) -1);
  float k=log(Rm/NTC_NOMINAL);
  float stack_temperature =(1.0/( ST_A + (ST_B*k) + (ST_C*k*k) + (ST_D*k*k*k)) ) -273.15 ;
  return stack_temperature;
}

/*
#define BCOEFFICIENT 3435 //kelvin
//Calculate stack_temperature using the Beta Factor equation
float ntc10k_beta(float ADC_CURRENT_VAL, uint16_t ADC_TOP_VAL)
{
  float stack_temperature;
  ADC_CURRENT_VAL= (ADC_TOP_VAL / ADC_CURRENT_VAL) - 1;
  ADC_CURRENT_VAL = RESISTOR_UP / ADC_CURRENT_VAL;
  //Calculate stack_temperature using the Beta Factor equation
  stack_temperature = ADC_CURRENT_VAL / NTC_NOMINAL;     // (R/Ro)
  stack_temperature = log(stack_temperature); // ln(R/Ro)
  stack_temperature /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  stack_temperature += 1.0 / (NTC_NOMINAL + 273.15); // + (1/To)
  stack_temperature = 1.0 / stack_temperature;
  stack_temperature -= 273.15;
  return stack_temperature;
  }
*/



