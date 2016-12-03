#ifndef MENU_FUELSENSOR_H_
#define MENU_FUELSENSOR_H_


void mainMenu_fuelSensor_job(void);

struct _taskman_fuelSensor
{
    int8_t sm0;
    int8_t sm1;
    int8_t sm2;
    int8_t sm3;
    //int8_t sm4;

    int8_t sign;
    int8_t reInitJob;
};
extern struct _taskman_fuelSensor taskman_fuelSensor;

#include <EEPROM.h>
#define fuelsensor_savedata() do{EEPROM.put(EEPROM_ADDR_FSEE, fuelsensor.fsEE);}while(0)

#endif // MENU_FUELSENSOR_H_
