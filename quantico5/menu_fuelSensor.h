#ifndef MENU_FUELSENSOR_H_
#define MENU_FUELSENSOR_H_

void mFuelSensor_warning_job(void);
void mFuelSensor_menu_job(void);


#include <EEPROM.h>
#define fuelsensor_savedata() do{EEPROM.put(EEPROM_ADDR_FSEE, fuelsensor.fsEE);}while(0)

#endif // MENU_FUELSENSOR_H_

