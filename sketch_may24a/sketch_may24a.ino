#include <EEPROM.h>

#define SPLINE_NODES_MAX 20

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
                uint8_t length;
                uint8_t width;
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
        unsigned lengthsTankdepth_consistent:1;
        unsigned area_spline_consistent:1;
        unsigned full_calib:1;
        unsigned zero_calib:1;
        unsigned __a:4;
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
void fuelsensor_job(void);
//
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


    int8_t i,j,pos;
    int8_t n;//max num node
    float Lmin, Lmax;//limits
    float yi;//output
    uint8_t innerlimits;




void setup() 
{
  // put your setup code here, to run once:

    struct _fuelsensor fuelsensor;
    fuelsensor.fsEE.bf.lengthsTankdepth_consistent = 1;
    fuelsensor.fsEE.bf.area_spline_consistent = 1;
    fuelsensor.fsEE.model = 55;
    
    float A[SPLINE_NODES_MAX];
    float B[SPLINE_NODES_MAX];
    float C[SPLINE_NODES_MAX];
    float D[SPLINE_NODES_MAX];

  for (int i=0; i<SPLINE_NODES_MAX; i++ )
  {
    A[i] = i;
    B[i] = i+2;
    C[i] = i+4;
    D[i] = i+6;
    }

    EEPROM.put(EEPROM_ADDR_FSEE, fuelsensor);
    
   //save  to eeprom
    #define EEPROM_ADDR_SPLINE_POLY_A (sizeof(struct _fuelsensor) + 0)
    #define EEPROM_ADDR_SPLINE_POLY_B (EEPROM_ADDR_SPLINE_POLY_A + sizeof(A))
    #define EEPROM_ADDR_SPLINE_POLY_C (EEPROM_ADDR_SPLINE_POLY_B + sizeof(B))
    #define EEPROM_ADDR_SPLINE_POLY_D (EEPROM_ADDR_SPLINE_POLY_C + sizeof(C))
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_A, A);
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_B, B);
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_C, C);
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_D, D);

    #define EEPROM_ADDR_SPLINE_POLY_LMIN (EEPROM_ADDR_SPLINE_POLY_D + sizeof(D))
    #define EEPROM_ADDR_SPLINE_POLY_LMAX (EEPROM_ADDR_SPLINE_POLY_LMIN + sizeof(float))
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_LMIN, Lmin);//float
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_LMAX, Lmax);


}

void loop() {
  // put your main code here, to run repeatedly:

}
