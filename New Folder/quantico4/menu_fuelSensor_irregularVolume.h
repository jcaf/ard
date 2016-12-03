#ifndef MENU_FUELSENSOR_IRREGULARVOLUME_H_INCLUDED
#define MENU_FUELSENSOR_IRREGULARVOLUME_H_INCLUDED

struct _menuFuelSensor_irregularVolume
{
    int sm0;
    int sm1;
} ;
extern struct _menuFuelSensor_irregularVolume menuFuelSensor_irregularVolume;


struct _menuFuelSensor_irregularVolume_errorConsistency
{
    int sm0;
    int sm1;
    int opt;
} ;
extern struct _menuFuelSensor_irregularVolume_errorConsistency menuFuelSensor_irregularVolume_errorConsistency;

struct _menuFuelSensor_irregularVolume_save_continue
{
    int sm0;
    int opt;
    int sm1;
} ;
extern struct _menuFuelSensor_irregularVolume_save_continue menuFuelSensor_irregularVolume_save_continue;


struct _menuFuelSensor_irregularVolume_minNode
{
    int sm0;
    int sm1;
    int opt;
} ;
extern struct _menuFuelSensor_irregularVolume_minNode menuFuelSensor_irregularVolume_minNode;

struct _menuFuelSensor_irregularVolume_maximum_size
{
    int sm0;
};
extern struct _menuFuelSensor_irregularVolume_maximum_size menuFuelSensor_irregularVolume_maximum_size;


void menuFuelSensor_irregularVolume_job(void);
void menuFuelSensor_irregularVolume_errorConsistency_job(void);
void menuFuelSensor_irregularVolume_save_continue_job(void);
void menuFuelSensor_irregularVolume_minNode_job(void);
void menuFuelSensor_irregularVolume_maximum_size_job(void);


struct _menuFuelSensor_irregular_mainMenu
{
    int8_t sm0;
};
extern struct _menuFuelSensor_irregular_mainMenu menuFuelSensor_irregular_mainMenu;

struct _menuFuelSensor_irregular_unitsOfTable
{
    int8_t sm0;
};

extern struct _menuFuelSensor_irregular_unitsOfTable menuFuelSensor_irregular_unitsOfTable;


struct _menuFuelSensor_irregular_unitsOfTable_lengthUnits
{
    int8_t sm0;
};
extern struct _menuFuelSensor_irregular_unitsOfTable_lengthUnits menuFuelSensor_irregular_unitsOfTable_lengthUnits;

struct _menuFuelSensor_irregular_unitsOfTable_volumenUnits
{
    int8_t sm0;
};

extern struct _menuFuelSensor_irregular_unitsOfTable_volumenUnits menuFuelSensor_irregular_unitsOfTable_volumenUnits;

void menuFuelSensor_irregular_mainMenu_job(void);
void menuFuelSensor_irregular_unitsOfTable_job(void);
void menuFuelSensor_irregular_unitsOfTable_lengthUnits_job(void);
void menuFuelSensor_irregular_unitsOfTable_volumenUnits_job(void);

#endif // MENU_FUELSENSOR_IRREGULARVOLUME_H_INCLUDED
