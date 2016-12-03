#include <Arduino.h>
#include "getString.h"
#include "system.h"

String getString_voltage(float V)
{
    char char_buff[DTOSTRF_SIZE_BUF];
    String str = String("V=") + dtostrf(V, DTOSTRF_WIDTH, DTOSTRF_PREC, char_buff) + "V";
    return str;
}
String getString_current(float I)
{
    char char_buff[DTOSTRF_SIZE_BUF];
    String str = String("I=") + dtostrf(I, DTOSTRF_WIDTH, DTOSTRF_PREC, char_buff) + "A";
    return str;
}
String getString_temperature(float T)
{
    char char_buff[DTOSTRF_SIZE_BUF];
    String str = String("T=") + dtostrf(T, DTOSTRF_WIDTH, DTOSTRF_PREC, char_buff) + "C";
    return str;
}

String getString_error(String Text, uint8_t E)
{
    String str = Text + String(E);//numError
    return str;
}

String getString_cell_voltage(float CV, uint8_t num_cell)
{
    char char_buff[DTOSTRF_SIZE_BUF];
    String str = String("C") + String(num_cell) + "=" + dtostrf(CV, DTOSTRF_WIDTH, DTOSTRF_PREC, char_buff) + "V";
    return str;
}
