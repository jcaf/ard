#ifndef FUELSENSOR_H_
#define FUELSENSOR_H_

#include "spline.h"

enum FS_OUTPUTTYPE {LENGTH=0, VOLUME};

struct _fsEEPROM
{
    uint8_t model;

    struct _outputType
    {
        FS_OUTPUTTYPE type;

        struct _length
        {
            uint8_t units;
        } length;

        struct _volume
        {
            uint8_t units;
        } volume;

    } outputType;

    struct _tank
    {
        uint8_t type;

        struct _rectangular
        {
            struct _area
            {
                float length;
                float width;
            } area;

        } rectangular;

        struct _irregular
        {
            struct _spline
            {
                struct _node
                {
                    float X;//x
                    float A;//f(x)

                } node[SPLINE_NODES_MAX];//#define SPLINE_NODES_MAX in spline.h

                uint8_t node_counter;//num_nodes

                struct _node_units
                {
                    struct _length
                    {
                        uint8_t units;//fixed to cm RESERVERD
                    }length;

                    struct _volume
                    {
                        uint8_t units;//gallons/liters
                    }volume;

                }node_units;

            } spline;

        } irregular;

    } tank;

    struct _calib//ration
    {
        struct _tank
        {
            float innertank;//d1 cm
            float zero2full;//d2 cm
            float full2upper;//d3 cm

        } tank;

    } calib;

    struct
    {
        unsigned full_calib:1;
        unsigned zero_calib:1;
        unsigned lengthsTankdepth_consistent:1;
        unsigned rectangular_area_consistent:1;
        unsigned irregular_spline_nodetable_consistent:1;
        unsigned enable_get_volume:1;
        unsigned __a:2;
    } bf;
};

struct _spline_polyvector
{
    float A[SPLINE_NODES_MAX];
    float B[SPLINE_NODES_MAX];
    float C[SPLINE_NODES_MAX];
    float D[SPLINE_NODES_MAX];
};


struct _fuelsensor
{
    float value;
    struct
    {
        unsigned do_fullzero:1;
        unsigned __a:7;

    } bf;
    struct _fsEEPROM fsEE;
};
#define EEPROM_ADDR_FSEE 0x0000
extern struct _fuelsensor fuelsensor;

void fuelsensor_init(void);
void fuelsensor_check_consistency(void);
void fuelsensor_job(void);
//

#define FS_MODEL_JT606X 0
//#define FS_MODEL_ 1
//#define FS_MODEL_ 2

#define FS_DO_FULL_CALIB 1
#define FS_DO_ZERO_CALIB 0
//
#define FS_UNITMEA_LENGTH_PERCENTAGE 0
#define FS_UNITMEA_LENGTH_CENTIMETERS 1
#define FS_UNITMEA_LENGTH_METERS 2

#define FS_UNITMEA_VOLUME_GALLONS 0
#define FS_UNITMEA_VOLUME_LITERS 1

#define FS_TANK_TYPE_RECTANGULAR 0
#define FS_TANK_TYPE_IRREGULAR 1

//negatives
#define FS_VALUE_OUT_LIMITS
#define FS_VALUE_OUT_LIMITS


String fuelsensor_reading(void);

void fuelsensor_inhibit_comm(void);
void fuelsensor_set_comm(void);


#endif // FUELSENSOR_H_

