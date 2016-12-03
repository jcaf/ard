#include <Arduino.h>
#include "openwire.h"
#include "LTC68042.h"
#include "ltc6804.h"

//safe time
#define TCYCLE_MD_FAST 5 //ms
#define TCYCLE_MD_NORMAL 5 //ms
#define TCYCLE_MD_FILTERED 210 //ms
//#define TSET_WRITE_CONFIG 0//85 -100ms no oficial

#define OPENWIRE_12V_ADC_MD_NORMAL MD_NORMAL
#define OPENWIRE_12V_ADC_MD_FILTERED MD_FILTERED

#define OPENWIRE_12V_DCP DCP_ENABLED//DCP_DISABLED
#define OPENWIRE_12V_ADC_MD OPENWIRE_12V_ADC_MD_NORMAL
//#define OPENWIRE_12V_ADC_MD OPENWIRE_12V_ADC_MD_FILTERED

#if OPENWIRE_12V_ADC_MD == OPENWIRE_12V_ADC_MD_NORMAL
#define TCYCLE_MD TCYCLE_MD_NORMAL
#elif OPENWIRE_12V_ADC_MD == OPENWIRE_12V_ADC_MD_FILTERED
#define TCYCLE_MD TCYCLE_MD_FILTERED
#endif

//#define OPENWIRE_12V_DEBUG_PRINT

static void openwire_12v_check_cellvolt_group(uint8_t num_ic, uint8_t rdcv_group,uint16_t * data_read);

uint8_t openwire_12v(uint8_t num_ic)
{
    #define CHECKOW12V_C1C0 0
    #define CHECKOW12V_C2C1 1
    #define CHECKOW12V_C3C2 2
    #define CHECKOW12V_C4C3 3

    static uint8_t checkow12v=CHECKOW12V_C1C0;
    static uint8_t sm0 = 0x00;
    static unsigned long last_millis;
    static uint8_t rdcv_group;
    //
    uint16_t data_read[3];//3 cellvolt x c/registro
    uint8_t finish;

    finish = 0;

    if (sm0 == 0)
    {
        if (checkow12v == CHECKOW12V_C1C0)
        {
            ltc_write_cfg_address(num_ic, uv_12v, ov_12v, 0x07<<0 );
            rdcv_group = READ_CELL_GROUP_A;
            checkow12v++;
        }
        else if (checkow12v == CHECKOW12V_C2C1)
        {
            ltc_write_cfg_address(num_ic, uv_12v, ov_12v, 0x07<<3 );
            rdcv_group = READ_CELL_GROUP_B;
            checkow12v++;
        }
        else if (checkow12v == CHECKOW12V_C3C2)
        {
            ltc_write_cfg_address(num_ic, uv_12v, ov_12v, 0x07<<6 );
            rdcv_group = READ_CELL_GROUP_C;
            checkow12v++;
        }
        else if (checkow12v == CHECKOW12V_C4C3)
        {
            ltc_write_cfg_address(num_ic, uv_12v, ov_12v, 0x07<<9 );
            rdcv_group = READ_CELL_GROUP_D;
            checkow12v = 0;
        }

        #ifdef OPENWIRE_12V_DEBUG_PRINT
        Serial.print(F("READ_CELL_GROUP_"));Serial.write('A'+ rdcv_group -1);Serial.println("");
        #endif


        set_adcv(OPENWIRE_12V_ADC_MD, OPENWIRE_12V_DCP, CELL_CH_ALL);

        LTC6804_ADCV_address(num_ic);

        last_millis = millis();
        sm0++;

    }

    //read_config();

//    if (sm0 == 1)
//    {
//        if ( (millis() - last_millis) > TSET_WRITE_CONFIG)//x write config (NO DATASHEET)
//        {
//            LTC6804_ADCV_address(num_ic);
//            last_millis = millis();
//            sm0++;
//        }
//    }

    if (sm0 == 1)//2)
    {
        if ( (millis() - last_millis) > TCYCLE_MD)
        {
            sm0 = 0x00;

            LTC6804_RDCV_BYGROUP_address(num_ic, rdcv_group, data_read);

            openwire_12v_check_cellvolt_group(num_ic, rdcv_group, data_read);

            #ifdef OPENWIRE_12V_DEBUG_PRINT
            Serial.print(F("num_ic:"));Serial.println(num_ic);Serial.print(F("rdcv_group:"));Serial.println(rdcv_group);
            for (uint8_t i=0; i<3; i++)
            {
                Serial.print(F("V"));Serial.print(i+1);Serial.print(F("  "));Serial.println(data_read[i]);
            }
            #endif

            if (rdcv_group == READ_CELL_GROUP_D)
            {
                finish = 1;
            }
        }
    }

    return finish;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t OPENWIRE_12V_THRESHOLD = 19000;//1.9V

static void openwire_12v_check_cellvolt_group(uint8_t num_ic, uint8_t rdcv_group,uint16_t * data_read)
{
    if ( (data_read[0] < OPENWIRE_12V_THRESHOLD) && (data_read[1] < OPENWIRE_12V_THRESHOLD) && (data_read[2] < OPENWIRE_12V_THRESHOLD) )
    {
        if (rdcv_group == READ_CELL_GROUP_A)
            openwire_flag[num_ic][0]=1;
        else if (rdcv_group == READ_CELL_GROUP_B)
            openwire_flag[num_ic][1]=1;
        else if (rdcv_group == READ_CELL_GROUP_C)
            openwire_flag[num_ic][2]=1;
        else if (rdcv_group == READ_CELL_GROUP_D)
            openwire_flag[num_ic][3]=1;
    }
    else
    {
        if (rdcv_group == READ_CELL_GROUP_A)
            openwire_flag[num_ic][0]=0;
        else if (rdcv_group == READ_CELL_GROUP_B)
            openwire_flag[num_ic][1]=0;
        else if (rdcv_group == READ_CELL_GROUP_C)
            openwire_flag[num_ic][2]=0;
        else if (rdcv_group == READ_CELL_GROUP_D)
            openwire_flag[num_ic][3]=0;
    }
}

/////////////////////////////////////////////////////////////////////////////////////
//OJO: como los IC se alimentan de su propio stack,
//en esa config. no es posible determinar si esta abierto el punto C0 y el C12
//Por lo tanto solo se puede determinar un openwire entre la celda 1 y la celda 11
//Y de manera certera siempre y cuando no existan mas de 3-4 celdas abiertas consecutivas.
/////////////////////////////////////////////////////////////////////////////////////

//#define OPENWIRE_2V_DEBUG_PRINT

#define OPENWIRE_2V_DCP DCP_DISABLED //DCP_ENABLED
#define OPENWIRE_2V_NUM_PASS 2//datasheet if adow is filtered mode

uint8_t openwire_2v(uint8_t num_ic)
{
    static uint16_t cell_PU[NUM_CELL_2V_MAX];
    static uint16_t cell_PD[NUM_CELL_2V_MAX];
    float cell_DIFF[NUM_CELL_2V_MAX];

    static uint8_t c_num_pass=0;
    static unsigned long last_millis;
    static uint8_t sm0=0;

    #define ADOW_STEP1 0
    #define ADOW_STEP2 1
    static uint8_t adow_step = ADOW_STEP1;

    uint8_t finish;

    finish=0;

    if(sm0 == 0)
    {
        if (adow_step == ADOW_STEP1)
            set_adow(MD_FILTERED, PULLUP_CURRENT, OPENWIRE_2V_DCP, CELL_CH_ALL );
        else
            set_adow(MD_FILTERED, PULLDOWN_CURRENT, OPENWIRE_2V_DCP, CELL_CH_ALL );

        sm0++;
    }

    if (sm0 == 1)
    {
        ltc_write_cfg_address(num_ic, uv_2v+FACTCORR_2V, ov_2v+FACTCORR_2V, 0x0000 );//uv ov irrelevant
        LTC6804_ADOW_address(num_ic);
        last_millis = millis();
        sm0++;
    }

    if (sm0 == 2)
    {
        if ( millis() - last_millis > 210)
        {
            c_num_pass++;

            if (c_num_pass >= OPENWIRE_2V_NUM_PASS)
            {
                c_num_pass = 0x00;

                if (adow_step == ADOW_STEP1)
                {
                    LTC6804_RDCV_BYGROUP_address(num_ic, READ_CELL_GROUP_ABCD, cell_PU);
                    sm0 = 0x00;
                    adow_step++;
                }
                else
                {
                    LTC6804_RDCV_BYGROUP_address(num_ic, READ_CELL_GROUP_ABCD, cell_PD);
                    adow_step = ADOW_STEP1;
                    sm0++;
                }
            }
            else
            {
                sm0 = 1;
            }
        }
    }

    if (sm0 > 2)
    {
        sm0=0;

        #ifdef OPENWIRE_2V_DEBUG_PRINT
        for (uint8_t num_cell=0; num_cell<NUM_CELL_2V_MAX; num_cell++)
        {
            Serial.print(F("IC address "));
            Serial.print(num_ic);
            Serial.print(F(" - CELDA PU"));
            Serial.print(num_cell+1);
            Serial.print(F(": "));
            Serial.println(cell_PU[num_cell]);
            Serial.print(F("IC address "));
            Serial.print(num_ic);
            Serial.print(F(" - CELDA PD"));
            Serial.print(num_cell+1);
            Serial.print(F(": "));
            Serial.println(cell_PD[num_cell]);
        }
        #endif

        //for (uint8_t num_cell=0; num_cell<NUM_CELL_2V_MAX; num_cell++)
        for (uint8_t num_cell=1; num_cell<NUM_CELL_2V_MAX; num_cell++)//1..11
        {
            cell_DIFF[num_cell] = (cell_PU[num_cell]/FACTDIV) - (cell_PD[num_cell]/FACTDIV);

            #ifdef OPENWIRE_2V_DEBUG_PRINT
            Serial.print(F("IC address "));
            Serial.print(num_ic);
            Serial.print(F(" - CELDA DIFF"));
            Serial.print(num_cell+1);
            Serial.print(F(": "));
            Serial.println(cell_DIFF[num_cell]);
            #endif
        }


        //for (uint8_t num_cell=1; num_cell<=10; num_cell++)//celda1..11
        for (uint8_t num_cell=0; num_cell<=10; num_cell++)//celda1..11
        {
            if (cell_DIFF[num_cell+1] < -0.4)//< -400mV
            {
                openwire_flag[num_ic][num_cell] = 1;

                #ifdef OPENWIRE_2V_DEBUG_PRINT
                Serial.print(F("IC address "));
                Serial.print(num_ic);
                Serial.print(F(" - SIN CONEXION: "));
                Serial.print(F("CELDA"));
                Serial.println(num_cell);
                #endif
            }
            else
            {
                openwire_flag[num_ic][num_cell] = 0;
            }
        }
        //
        //add
        //
        if ( ( (float)(cell_PU[0] /FACTDIV ) ) <= 0.500) //< 0.1)
        {
            // external powered
            //ltc_connected[num_ic] = LTC_DISCONNECTED;
        }
        else
        {
            //ltc_connected[num_ic] = LTC_CONNECTED;
        }


         if ( (  (float)(cell_PD[11] /FACTDIV) ) == 0.000) // < 0.1)
            openwire_flag[num_ic][11] = 1;
         else
            openwire_flag[num_ic][11] = 0;
        //

        finish =1;
    }

    return finish;
}


