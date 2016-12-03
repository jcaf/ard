#include <Arduino.h>
#include "utils.h"

uint8_t dec2bcd(uint8_t dec)
{
    return ( (dec/10)<<4 ) + (dec%10);
}

char bin_to_asciihex(char c)
{
    if (c < 10)
        return c+'0';
    else
        return (c-10) + 'A';
}

void dtostrf_fill_zero(char *p)
{
    while (*p !='.')
    {
        if (*p == ' ')
            *p='0';
        p++;
    }
}
