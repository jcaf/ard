#ifndef MENU_FUELSENSOR_OUTPUTTYPEUNITS_H_INCLUDED
#define MENU_FUELSENSOR_OUTPUTTYPEUNITS_H_INCLUDED

struct _menuFuelSensor_outputType
{
    int8_t sm0;
} ;
extern struct _menuFuelSensor_outputType menuFuelSensor_outputType;


struct _menuFuelSensor_volumenUnits
{
    int8_t sm0;
} ;
extern struct _menuFuelSensor_volumenUnits menuFuelSensor_volumenUnits;

struct _menuFuelSensor_lengthUnits
{
    int8_t sm0;
};
extern struct _menuFuelSensor_lengthUnits menuFuelSensor_lengthUnits;


void menuFuelSensor_outputType_job(void);
void menuFuelSensor_lengthUnits_job(void);
void menuFuelSensor_volumenUnits_job(void);




#endif // MENU_FUELSENSOR_OUTPUTTYPEUNITS_H_INCLUDED
