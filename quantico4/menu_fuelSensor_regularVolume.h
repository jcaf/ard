#ifndef MENU_FUELSENSOR_REGULARVOLUME_H_INCLUDED
#define MENU_FUELSENSOR_REGULARVOLUME_H_INCLUDED

struct _menuFuelSensor_rectangularVolume
{
    int sm0;
    //int sm1;
};
extern struct _menuFuelSensor_rectangularVolume menuFuelSensor_rectangularVolume;

struct _menuFuelSensor_rectangularVolumen_save_and_exit
{
    int sm0;
    int sm1;
};
extern struct _menuFuelSensor_rectangularVolumen_save_and_exit menuFuelSensor_rectangularVolumen_save_and_exit;



void menuFuelSensor_rectangularVolume_job(void);
void menuFuelSensor_rectangularVolumen_save_and_exit_job(void);


#endif // MENU_FUELSENSOR_REGULARVOLUME_H_INCLUDED
