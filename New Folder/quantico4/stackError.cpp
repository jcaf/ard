#include <Arduino.h>
#include "stackError.h"
#include "LTC68042.h"
#include "ltc6804.h"

uint8_t get_stackError_UV(uint8_t num_ic)
{
    uint8_t num_cell, NUM_CELL_TOP;
    uint8_t numError_UV = 0x00;

    if (select_2_12V[num_ic] == 2)
        NUM_CELL_TOP = NUM_CELL_2V_MAX;
    else
        NUM_CELL_TOP = NUM_CELL_12V_MAX;


    for (num_cell = 0; num_cell< NUM_CELL_TOP ; num_cell++)
         if(uv_flag[num_ic][num_cell] == 1)
            numError_UV++;

    return numError_UV;
}

uint8_t get_stackError_OV(uint8_t num_ic)
{
    uint8_t num_cell, NUM_CELL_TOP;
    uint8_t numError_OV = 0x00;

    if (select_2_12V[num_ic] == 2)
        NUM_CELL_TOP = NUM_CELL_2V_MAX;
    else
        NUM_CELL_TOP = NUM_CELL_12V_MAX;


    for (num_cell = 0; num_cell< NUM_CELL_TOP ; num_cell++)
         if(ov_flag[num_ic][num_cell] == 1)
            numError_OV++;

    return numError_OV;
}

uint8_t get_stackError_openWire(uint8_t num_ic)
{
    uint8_t num_cell, NUM_CELL_TOP;
    uint8_t numError_openWire = 0x00;

    if (select_2_12V[num_ic] == 2)
        NUM_CELL_TOP = NUM_CELL_2V_MAX;
    else
        NUM_CELL_TOP = NUM_CELL_12V_MAX;

    for (num_cell = 0; num_cell< NUM_CELL_TOP ; num_cell++)
         if(openwire_flag[num_ic][num_cell] == 1)
            numError_openWire++;

    return numError_openWire;
}

uint8_t get_stackError_overCurrent(uint8_t num_ic)
{
    return stack_overCurrent[num_ic];
}
uint8_t get_stackError_overTemperature(uint8_t num_ic)
{
    return stack_overTemperature[num_ic];
}

uint8_t get_stackError_total(uint8_t num_ic)
{
    return  get_stackError_UV(num_ic)+
            get_stackError_OV(num_ic)+
            get_stackError_openWire(num_ic)+
            get_stackError_overCurrent(num_ic)+
            get_stackError_overTemperature(num_ic);
}

