#ifndef MENU_FUELSENSOR_CALIBRATION_H_INCLUDED
#define MENU_FUELSENSOR_CALIBRATION_H_INCLUDED


struct _menuFuelSensor_calibration
{
    int8_t sm0;
} ;
extern struct _menuFuelSensor_calibration  menuFuelSensor_calibration;


struct _menuFuelSensor_lengthTankDepth_FullZeroCalibration
{
    int sm0;
    int sm1;

};
extern struct _menuFuelSensor_lengthTankDepth_FullZeroCalibration menuFuelSensor_lengthTankDepth_FullZeroCalibration;


struct _menuFuelSensor_lengthTankDepth
{
    int sm0;

};
extern struct _menuFuelSensor_lengthTankDepth menuFuelSensor_lengthTankDepth;


struct _menuFuelSensor_lengthTankDepth_errorConsistency
{
    int sm0;
    int sm1;
};
extern struct _menuFuelSensor_lengthTankDepth_errorConsistency menuFuelSensor_lengthTankDepth_errorConsistency;

struct _menuFuelSensor_lengthTankDepth_noErrorConsistency
{
    int sm0;
    int sm1;
};
extern struct _menuFuelSensor_lengthTankDepth_noErrorConsistency menuFuelSensor_lengthTankDepth_noErrorConsistency;

void menuFuelSensor_calibration_job(void);
void menuFuelSensor_lengthTankDepth_FullZeroCalibration_job(void);
void menuFuelSensor_lengthTankDepth_job(void);
void menuFuelSensor_lengthTankDepth_errorConsistency_job(void);
void menuFuelSensor_lengthTankDepth_noErrorConsistency_job(void);



#endif // MENU_FUELSENSOR_CALIBRATION_H_INCLUDED
