#ifndef MENU_FUELSENSOR_FUELTANK_H_INCLUDED
#define MENU_FUELSENSOR_FUELTANK_H_INCLUDED


struct _menuFuelSensor_tank
{
    int sm0;

};
extern struct _menuFuelSensor_tank menuFuelSensor_tank;

struct _menuFuelSensor_typeOfTank
{
    int sm0;
};
extern struct _menuFuelSensor_typeOfTank menuFuelSensor_typeOfTank;

void menuFuelSensor_tank_job(void);
void menuFuelSensor_typeOfTank_job(void);

#endif // MENU_FUELSENSOR_TYPEOFTANK_H_INCLUDED

