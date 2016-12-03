#include <Arduino.h>
#include <SPI.h>
#include "system.h"
#include "LTC68042.h"
#include "ltc6804.h"
#include "temperature.h"
#include "openwire.h"
#include "stackcurrent.h"

float   vcellF[NUM_IC][LTC_NUM_CELLS];
float   vgpioF[NUM_IC][NUM_TOTAL_REGAUX];

uint8_t ltc_connected[NUM_IC];
uint8_t stack_union[NUM_IC];
uint8_t select_2_12V[NUM_IC];

float   stack_temperatureF[NUM_IC];
float   stack_voltage_ifcF[NUM_IC];//ifc=included factor corr.
float   stack_currentF[NUM_IC];

uint8_t uv_flag[NUM_IC][LTC_NUM_CELLS];
uint8_t ov_flag[NUM_IC][LTC_NUM_CELLS];
uint8_t openwire_flag[NUM_IC][LTC_NUM_CELLS];
uint8_t stack_overCurrent[NUM_IC];
uint8_t stack_overTemperature[NUM_IC];

float   internal_digital_powerF[NUM_IC];//digital power supply, nominal:[2.7V-3.6V]
float   internal_analog_powerF[NUM_IC];//analog power supply, nominal: [4.5V-5.5V]
float   internal_die_tempF[NUM_IC];
float   internal_socF[NUM_IC];//SOC

static uint8_t ltc_get_connectedStatus(void);
static void ltc_set_connectedStatus(uint16_t (*gpio_matrix)[NUM_TOTAL_REGAUX]);
static void ltc_ifRemove_dataFlush(void);

static void parse_gpio(uint16_t (*gpio_matrix)[NUM_TOTAL_REGAUX]);
static void parse_gpio_debug_print(uint16_t (*gpio_matrix)[NUM_TOTAL_REGAUX]);

static uint8_t openwire(void);
static void read_status(void);
static uint8_t read_cellvoltage(void);
static void parse_cellvoltage(uint8_t num_ic, uint16_t data_read[LTC_NUM_CELLS]);

static void monitor_stack_current(void);
static void monitor_stack_temperature(void);

static void openwire_print2user(uint8_t num_ic);
static void cellvoltage_print2user(uint8_t num_ic);
static void status_print2user(uint8_t num_ic);
static void aux_print2user(uint8_t num_ic);
static void monitor_stack_current_print2user(uint8_t num_ic);
static void monitor_stack_temperature_print2user(uint8_t num_ic);
static void stack_union_print2user(uint8_t num_ic);

//FACTOR CORRECTION based FLUKE 175
float FACTCORR_2V = +0.0042;// + 0.0202 LTC genera una caida de tension

float FACTCORR_12V_C1 = +0.073;
float FACTCORR_12V_C2 = +0.064;
float FACTCORR_12V_C3 = +0.047;
float FACTCORR_12V_C4 = +0.067;

//VUV = VOV = 12 BITS = 0x0FFF = 4095
//ADC Range aprox. -0.8192V -> 5.7344VV = 65536 = 2^16
//ov_2v / uv_2v  Usados por hardware
float ov_2v = 3.500 + FACTCORR_2V;//<= 5.73V
float uv_2v = 1.000 + FACTCORR_2V;//>= 0.0016V

//ov_12v_S3C / uv_12v_S3C Usados por software
float ov_12v_S3C = 13.500;//S3C Sum of 3 Cell
float uv_12v_S3C = 10.0;//1.950;


//ov_12v / uv_12v  usados SOLO de manera formal para CONFIG REGISTER
//float FACTCORR_12V = (FACTCORR_12V_C1+FACTCORR_12V_C2+FACTCORR_12V_C3+FACTCORR_12V_C4)/4;
float ov_12v = 5.5;// + FACTCORR_12V;//<= 5.73V
float uv_12v = 2.5;// + FACTCORR_12V;//>= 0.0016V

#define LTC_JOB_PRINT2USER
static void ltc_job_print2user(void);

////////////////////////////////////////////////////////////
/////////////////////////
//LTC6804- max velocidad SPI = 1/(t3=200ns+t4=200ns) = 2.5MHz
//
//Segun LTC6820, el max es 1MHz hasta 10metros.
//mejor bajar la velocidad a 500kHz x compatibilidad a futuro hasta 100 metros
//ATmega2560 @16MHz -> fosc/32 = 0.5MHz
/////////////////////////////////////////////////////////////////////////////////////

#define LTC6804_SPI_SPEED 500E3//500KHz

void ltc6804_init(void)
{
    digitalWrite(WIZNET_CS, HIGH);
    pinMode(WIZNET_CS, OUTPUT);

    digitalWrite(SDCARD_CS, HIGH);
    pinMode(SDCARD_CS, OUTPUT);

    digitalWrite(LTC6804_CS, HIGH);
    pinMode(LTC6804_CS, OUTPUT);

    //pinMode(ATMEGA_SPI_SS,OUTPUT);//SPI.begin se encarga de poner SS x hardware como salida

    SPI.beginTransaction(SPISettings(LTC6804_SPI_SPEED, MSBFIRST, SPI_MODE3));
    SPI.begin();//begin is necessary for ON of SPI MASTER -> SPI/MSTR=1, SCK=MOSI=OUTPUT

}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
struct _ltc6804_app ltc6804;
uint8_t ltc_connected_toEnter[NUM_IC];

uint8_t ltc6804_job(void)
{
    static uint8_t sm0, sm1, sm2, sm3;
    static unsigned long last_millis;

    uint16_t gpio_matrix[NUM_IC][NUM_TOTAL_REGAUX];
    uint8_t finish = 0;

    if (sm0 == 0)
    {
        if (sm1 == 0)
        {
            set_adax(MD_FILTERED, AUX_CH_ALL);
            LTC6804_ADAX_broadcast();
            last_millis = millis();

            sm1++;
        }
        if (sm1 == 1)
        {
            if ( (millis() - last_millis) > 210)
            {
                ltc_rdaux_groupAB_allIC(gpio_matrix);
                ltc_set_connectedStatus(gpio_matrix);
                parse_gpio(gpio_matrix);

                sm1= 0x00;
                sm0++;
            }
        }
    }

    if (sm0 == 1)
    {
        if (sm2 == 0)
        {
            if (ltc_get_connectedStatus())
                sm2++;
        }
        if (sm2 == 1)
        {
            ltc6804.app.synch.data_invalid=1;

            if (sm3 == 0)
            {
                if (read_cellvoltage()) //STATUS REGISTER "B" will be update (UV OV )
                    sm3++;
            }
            else if (sm3 == 1)
            {
                if (openwire())
                    sm3++;
            }
            else if (sm3 == 2)
            {
                set_stat(MD_FILTERED , STAT_CH_ALL);
                LTC6804_ADSTAT_broadcast(); //SOC, ITMP, analog (VA) and digital (VD)
                last_millis = millis();
                sm3++;
            }
            else if (sm3 == 3)
            {
                if ( (millis() - last_millis) > (135 + 10) ) //134,218 us
                {
                    read_status();
                    sm3++;
                }
            }
            else if (sm3 == 4)
            {
                //monitor_stack_temperature();//RESERVED
                monitor_stack_current();

                sm3 = 0x00;

            }
            sm2++;
        }

        if (sm2 == 2)
        {
            //if (1)//
            if (ltc_get_connectedStatus())
            {
                ltc_ifRemove_dataFlush();

                //finish = 1;
                ltc6804.app.synch.data_invalid=0;

                //sm0 = 0;
                //sm2 = 0;

                //+-Add x temperature stabilizing
                sm2++;
                last_millis = millis();
                //-+
            }

        }

        //+- delay(200);//x temperature stabilizing
        if (sm2 == 3)
        {
            if ( (millis() - last_millis) > 190 )
            {
                sm0=0;
                sm2=0;
                finish = 1;
            }
        }
        //-+


    }
//delay(200);


#ifdef LTC_JOB_PRINT2USER
    if (sm0 == 0x00)
    {
        ltc_job_print2user();
    }
#endif // LTC_JOB_PRINT2USER

    return finish;
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
#ifdef LTC_JOB_PRINT2USER
static void ltc_job_print2user(void)
{
    for (uint8_t num_ic = 0; num_ic < NUM_IC; num_ic++)
    {
        if (ltc_connected[num_ic] == LTC_CONNECTED)
        {
            //__header:
            Serial.println(F("____________________________________________"));
            Serial.print(F("IC ADDRESS:")); Serial.print(num_ic); Serial.print(F(" [")); Serial.print(select_2_12V[num_ic]); Serial.println(F("V] "));
            //__

            Serial.println(F(""));

            openwire_print2user(num_ic);
            cellvoltage_print2user(num_ic);
            aux_print2user(num_ic);
            status_print2user(num_ic);
            stack_union_print2user(num_ic);
            monitor_stack_current_print2user(num_ic);
            monitor_stack_temperature_print2user(num_ic);
        }
    }
}
#endif // LTC_JOB_PRINT2USER

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static uint8_t ltc_get_connectedStatus(void)
{
    static uint8_t sm0;
    static unsigned long last_millis;

    uint16_t gpio_matrix[NUM_IC][NUM_TOTAL_REGAUX];
    uint8_t finish=0;

    if (sm0 == 0)
    {
        set_adax(MD_FAST, AUX_CH_ALL);
        LTC6804_ADAX_broadcast();
        last_millis = millis();

        sm0++;
    }
    if (sm0 == 1)
    {
        if ( (millis() - last_millis) > 4)//2)//2ms
        {
            ltc_rdaux_groupAB_allIC(gpio_matrix);
            ltc_set_connectedStatus(gpio_matrix);

            sm0 = 0;
            finish = 1;
        }
    }
    return finish;
}
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static void ltc_set_connectedStatus(uint16_t (*gpio_matrix)[NUM_TOTAL_REGAUX])//
{
    uint8_t count_presence;
    for (uint8_t num_ic = 0; num_ic < NUM_IC; num_ic++)
    {
        count_presence = 0x00;
        for (uint8_t num_regaux = 0; num_regaux < NUM_TOTAL_REGAUX ; num_regaux++)
            if (gpio_matrix[num_ic][num_regaux] == 0xFFFF)
                count_presence++;

        if (count_presence == NUM_TOTAL_REGAUX )
            ltc_connected[num_ic] = LTC_DISCONNECTED;
        else
            ltc_connected[num_ic] = LTC_CONNECTED;

        ltc_connected_toEnter[num_ic] = ltc_connected[num_ic];
    }

// Serial.println("ltc_set_connectedStatus -  begin");
//     for (uint8_t num_ic = 0; num_ic < NUM_IC; num_ic++)
//     {
//         Serial.print(F("IC address:")); Serial.println(num_ic);
//         for (uint8_t num_regaux = 0; num_regaux < NUM_TOTAL_REGAUX ; num_regaux++)
//         {Serial.println(gpio_matrix[num_ic][num_regaux]);}
//         Serial.println(F(""));
//     }
// Serial.println("ltc_set_connectedStatus -  end");
}
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//#define PARSE_GPIO_DEBUG_PRINT
static void parse_gpio(uint16_t (*gpio_matrix)[NUM_TOTAL_REGAUX])//
{
    for (uint8_t num_ic = 0; num_ic < NUM_IC; num_ic++)
    {
        if (ltc_connected[num_ic] == LTC_CONNECTED)
        {
            //1.-cell -> float
            for (int num_regaux = 0; num_regaux < NUM_TOTAL_REGAUX; num_regaux++)
            {
                vgpioF[num_ic][num_regaux] =  (gpio_matrix[num_ic][num_regaux] / FACTDIV);
            }

            //2.- Parse DIP switch
            //check  Select2_12V
            if (vgpioF[num_ic][AUX_GPIO_SELECT_2_12V] > 4.0)
                select_2_12V[num_ic] = 12;
            else if (vgpioF[num_ic][AUX_GPIO_SELECT_2_12V] < 1.0)
                select_2_12V[num_ic] = 2;

            //check stack_union
            if (vgpioF[num_ic][AUX_STACK_UNION] > 4.0)
                stack_union[num_ic] = 0;
            else if (vgpioF[num_ic][AUX_STACK_UNION] < 0.8)
                stack_union[num_ic] = 1;

            //3.- get stack_temperatureF
            stack_temperatureF[num_ic] = ntc10k_st( gpio_matrix[num_ic][AUX_GPIO_NTC10K], gpio_matrix[num_ic][AUX_LTC_VREF2] );

            //4.- get stack current
            stack_currentF[num_ic] = get_stack_current(gpio_matrix[num_ic][AUX_GPIO_CURRENTSENSE]);
        }

    }
#ifdef PARSE_GPIO_DEBUG_PRINT

    for (uint8_t num_ic = 0; num_ic < NUM_IC; num_ic++)
    {
        Serial.print(F("IC address:")); Serial.println(num_ic);
        for (uint8_t num_regaux = 0; num_regaux < NUM_TOTAL_REGAUX ; num_regaux++)
            {Serial.println(gpio_matrix[num_ic][num_regaux]);}
        Serial.println(F(""));
    }

/*
for (uint8_t num_ic = 0; num_ic < NUM_IC; num_ic++)
    Serial.println(gpio_matrix[num_ic][AUX_GPIO_NTC10K]);
        Serial.println(F(""));
*/
#endif // PARSE_GPIO_DEBUG_PRINT
}
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static void ltc_ifRemove_dataFlush(void)
{
    for (uint8_t num_ic = 0; num_ic < NUM_IC; num_ic++)
    {
        if (ltc_connected_toEnter[num_ic] != ltc_connected[num_ic])
        {
            ltc_connected_toEnter[num_ic] = ltc_connected[num_ic];

            for (uint8_t num_cell = 0; num_cell < LTC_NUM_CELLS; num_cell++)
            {
                vcellF[num_ic][num_cell] = 0;
                uv_flag[num_ic][num_cell] = 0;
                ov_flag[num_ic][num_cell] = 0;
                openwire_flag[num_ic][num_cell] = 0;
            }
            for (uint8_t num_regaux = 0; num_regaux < NUM_TOTAL_REGAUX; num_regaux++)
                vgpioF[num_ic][num_regaux] = 0;


            stack_temperatureF[num_ic] = 0;
            stack_voltage_ifcF[num_ic] = 0;
            stack_currentF[num_ic] = 0;
            stack_overCurrent[num_ic] = 0;
            internal_digital_powerF[num_ic] = 0;
            internal_analog_powerF[num_ic] = 0;
            internal_die_tempF[num_ic] = 0;
            internal_socF[num_ic] = 0;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static uint8_t openwire(void)
{
    static uint8_t num_ic = 0;

    uint8_t finish;
    uint8_t finish_2_12V;

    finish = 0;

    if (ltc_connected[num_ic] == LTC_CONNECTED )
    {
        if (select_2_12V[num_ic] == 2)
            finish_2_12V = openwire_2v(num_ic);
        else
            finish_2_12V = openwire_12v(num_ic);

        if (finish_2_12V)
        {
            num_ic++;
            goto __openwire_num_ic;
        }
    }
    else
    {
        num_ic++;
    }

__openwire_num_ic:
    if (num_ic == NUM_IC)
    {
        num_ic = 0x00;
        finish = 1;
    }

    return finish;
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static void openwire_print2user(uint8_t num_ic)
{
    uint8_t NUM_CELL_TOP;

    if (select_2_12V[num_ic] == 2)
        NUM_CELL_TOP = NUM_CELL_2V_MAX - 1; //CELDA 1-11
    else
        NUM_CELL_TOP = NUM_CELL_12V_MAX;

    Serial.println(F("[ OPENWIRE ]"));
    //Serial.println(F(""));

    for (uint8_t num_cell = 0; num_cell < NUM_CELL_TOP; num_cell++)
    {
        if (openwire_flag[num_ic][num_cell])
        {
            Serial.print(F("sin conexion: ")); Serial.print(F("celda ")); Serial.println(num_cell + 1);
        }
    }
    Serial.println(F(""));
}

/////////////////////////////////////////////////////////////////////////////////////
//uv ov flags are valid after a LTC6804_ADCV_address
/////////////////////////////////////////////////////////////////////////////////////
//#define READ_CELLVOLTAGE_DEBUG_PRINT
static uint8_t read_cellvoltage(void)
{
    static uint8_t num_ic = 0;
    static uint8_t sm0 = 0;
    static unsigned long last_millis;

    uint16_t data_read[LTC_NUM_CELLS];
    uint8_t finish;

    finish = 0;

    if (ltc_connected[num_ic] == LTC_CONNECTED)
    {
        if (sm0 == 0)
        {
            if (select_2_12V[num_ic] == 2)
            {
                ltc_write_cfg_address(num_ic, uv_2v, ov_2v, 0x0000);
                set_adcv(MD_FILTERED, DCP_DISABLED, CELL_CH_ALL);
            }
            else
            {
                ltc_write_cfg_address(num_ic, uv_12v, ov_12v, 0x0FFF);
                set_adcv(MD_FILTERED, DCP_ENABLED, CELL_CH_ALL);
            }

            LTC6804_ADCV_address(num_ic);//Whenever the C inputs are measured, the results are compared to uv and ov stored in memory

            last_millis = millis();
            sm0++;
        }

        if (sm0 == 1)
        {
            if ( (millis() - last_millis) > 210)
            {
                LTC6804_RDCV_BYGROUP_address(num_ic, READ_CELL_GROUP_ABCD, data_read);

#ifdef READ_CELLVOLTAGE_DEBUG_PRINT
                Serial.print(F("IC ADDRESS:")); Serial.print(num_ic); Serial.print(F(" [")); Serial.print(select_2_12V[num_ic]); Serial.println(F("V] "));
                Serial.println(F("read_cellvoltage (LTC6804_RDCV_BYGROUP_address) 1-12:")); Serial.println(F(""));
                for (uint8_t num_cell = 0; num_cell < LTC_NUM_CELLS; num_cell++)
                {Serial.print(F("celda")); Serial.print(num_cell + 1); Serial.print(F(": ")); Serial.println(data_read[num_cell]);}
                Serial.println(F(""));
#endif

                parse_cellvoltage(num_ic, data_read);//updated vcellF

                sm0 = 0x00;

                num_ic++;
                goto __read_cellvoltage_check_num_ic;
            }
        }
    }
    else
    {
        num_ic++;
    }

__read_cellvoltage_check_num_ic:
    if (num_ic == NUM_IC)
    {
        num_ic = 0x00;
        finish = 1;
    }

    return finish;
}
/////////////////////////////////////////////////////////////////////////////////////
//Aplicar los factores de correccion
/////////////////////////////////////////////////////////////////////////////////////
static void parse_cellvoltage(uint8_t num_ic, uint16_t data_read[LTC_NUM_CELLS])
{
    for (int num_cell = 0; num_cell < LTC_NUM_CELLS; num_cell++)
    {
        vcellF[num_ic][num_cell] =  (data_read[num_cell] / FACTDIV);
    }

    if (select_2_12V[num_ic] == 2)
    {
        stack_voltage_ifcF[num_ic] = 0;

        for (int num_cell = 0; num_cell < LTC_NUM_CELLS; num_cell++)
        {
            vcellF[num_ic][num_cell] += FACTCORR_2V;

            stack_voltage_ifcF[num_ic] += vcellF[num_ic][num_cell];
        }
    }
    else
    {
        float C1_12V = ((vcellF[num_ic][0]) ) + ((vcellF[num_ic][1]) ) + ((vcellF[num_ic][2]) ) + FACTCORR_12V_C1;
        float C2_12V = ((vcellF[num_ic][3]) ) + ((vcellF[num_ic][4]) ) + ((vcellF[num_ic][5]) ) + FACTCORR_12V_C2;
        float C3_12V = ((vcellF[num_ic][6]) ) + ((vcellF[num_ic][7]) ) + ((vcellF[num_ic][8]) ) + FACTCORR_12V_C3;
        float C4_12V = ((vcellF[num_ic][9]) ) + ((vcellF[num_ic][10]) ) + ((vcellF[num_ic][11]) ) + FACTCORR_12V_C4;

        vcellF[num_ic][0] = C1_12V;
        vcellF[num_ic][1] = C2_12V;
        vcellF[num_ic][2] = C3_12V;
        vcellF[num_ic][3] = C4_12V;

        stack_voltage_ifcF[num_ic] = vcellF[num_ic][0] + vcellF[num_ic][1] + vcellF[num_ic][2] + vcellF[num_ic][3];

        for (uint8_t num_cell = 4; num_cell < LTC_NUM_CELLS; num_cell++)
        {vcellF[num_ic][num_cell] = 0;} //clear unused positions
    }
}
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static void cellvoltage_print2user(uint8_t num_ic)
{
    uint8_t NUM_CELL_TOP;
    uint8_t NUM_DECIMALS;

    if (select_2_12V[num_ic] == 2)
    {
        NUM_CELL_TOP = NUM_CELL_2V_MAX;
        NUM_DECIMALS = NUM_DECIMALS_2V;
    }
    else
    {
        NUM_CELL_TOP = NUM_CELL_12V_MAX;
        NUM_DECIMALS = NUM_DECIMALS_GRL;//NUM_DECIMALS_12V;
    }


    Serial.println(F("[ CELL VOLTAGE ]"));

    for (uint8_t num_cell = 0; num_cell < NUM_CELL_TOP; num_cell++)
    {
        Serial.print(F("celda")); Serial.print(num_cell + 1); Serial.print(F(": "));
        Serial.print( vcellF[num_ic][num_cell], NUM_DECIMALS); Serial.println(F(" V"));
    }
    Serial.println(F(""));

    Serial.print(F("stack total voltage: ")); Serial.print(stack_voltage_ifcF[num_ic], NUM_DECIMALS); Serial.println(F(" V"));
    Serial.println(F(""));
}


/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
#define STA_SOC_L   0
#define STA_SOC_H   1
#define STA_ITMP_L  2
#define STA_ITMP_H  3
#define STA_VA_L    4
#define STA_VA_H    5

#define STB_VD_L                    0
#define STB_VD_H                    1
#define STB_OVUV_4_1                2
#define STB_OVUV_8_5                3
#define STB_OVUV_12_9               4
#define STB_REV_RSVB_MUXFAIL_THSD   5

#define READ_STATUS_DEBUG_PRINT

static void read_status(void)
{
    uint8_t data[NUM_BYTES_IN_REGGROUP];
    float dietemp;
    uint8_t c, cbit;

    for (uint8_t num_ic = 0; num_ic < NUM_IC; num_ic++)
    {
        if (ltc_connected[num_ic] == LTC_CONNECTED)
        {
            LTC6804_RDSTAT_address(num_ic, 'A', data);

#ifdef READ_STATUS_DEBUG_PRINT
            Serial.print(F("IC ADDRESS")); Serial.print(num_ic); Serial.println(F(" - STATA"));
            for (int i = 0; i < NUM_BYTES_IN_REGGROUP; i++)
            {Serial.print(F("reg")); Serial.print(i); Serial.print(F(": "));  Serial.println(data[i], HEX); }
            Serial.println(F(""));
#endif // READ_STATUS_DEBUG_PRINT

            internal_socF[num_ic] = (( ((uint16_t)data[STA_SOC_H] << 8) | data[STA_SOC_L]) * 20.0) / FACTDIV;
            dietemp = (((((uint16_t)data[STA_ITMP_H] << 8) | data[STA_ITMP_L]) / FACTDIV) / 7.5e-3) - 273;
            internal_die_tempF[num_ic] = dietemp;
            internal_analog_powerF[num_ic] = ( ((uint16_t)data[STA_VA_H] << 8) | data[STA_VA_L]) / FACTDIV;


            LTC6804_RDSTAT_address(num_ic, 'B', data);

#ifdef READ_STATUS_DEBUG_PRINT
            Serial.print(F("IC ADDRESS")); Serial.print(num_ic); Serial.println(F(" - STATB"));
            for (int i = 0; i < NUM_BYTES_IN_REGGROUP; i++)
            {Serial.print(F("reg")); Serial.print(i); Serial.print(F(": "));  Serial.println(data[i], HEX); }
            Serial.println(F(""));
#endif // READ_STATUS_DEBUG_PRINT

            internal_digital_powerF[num_ic] = ( ( (uint16_t)data[STB_VD_H] << 8) | data[STB_VD_L]) / FACTDIV;

            if (select_2_12V[num_ic] == 2)
            {
                //__
                c = 0;
                for (uint8_t index = STB_OVUV_4_1; index <= STB_OVUV_12_9; index++)
                {
                    cbit = 0;

                    for (int num_it = 0; num_it < 4; num_it++)
                    {
                        uv_flag[num_ic][c] = (data[index] & (1 << cbit)) >> cbit;
                        cbit++;
                        ov_flag[num_ic][c] = (data[index] & (1 << cbit)) >> cbit;
                        cbit++;
                        //
                        c++;
                    }
                }
                //__
            }
            else
            {
                for (uint8_t num_cell = 0; num_cell < NUM_CELL_12V_MAX; num_cell++)
                {
                    if (vcellF[num_ic][num_cell] < uv_12v_S3C)
                        uv_flag[num_ic][num_cell] = 1;
                    else
                        uv_flag[num_ic][num_cell] = 0;

                    if (vcellF[num_ic][num_cell] > ov_12v_S3C)
                        ov_flag[num_ic][num_cell] = 1;
                    else
                        ov_flag[num_ic][num_cell] = 0;
                }


            }
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static void status_print2user(uint8_t num_ic)
{
    uint8_t NUM_CELL_TOP;

    Serial.println(F("[ STATUS REGISTERS ]"));

    Serial.print(F("internal Sum Of Cell (SOC): ")); Serial.print(internal_socF[num_ic], NUM_DECIMALS_GRL); Serial.println(F(" V"));
    Serial.print(F("internal die temp.: ")); Serial.print(internal_die_tempF[num_ic], NUM_DECIMALS_GRL); Serial.println(F(" C"));
    Serial.print(F("internal analog VA (4.5V-5.5V): ")); Serial.print(internal_analog_powerF[num_ic], NUM_DECIMALS_GRL); Serial.println(F(" V"));
    Serial.print(F("internal digital VD (2.7V-3.6V): ")); Serial.print(internal_digital_powerF[num_ic], NUM_DECIMALS_GRL); Serial.println(F(" V"));

    Serial.println(F(""));

    if (select_2_12V[num_ic] == 2)
        NUM_CELL_TOP = NUM_CELL_2V_MAX;
    else
        NUM_CELL_TOP = NUM_CELL_12V_MAX;

    for (uint8_t num_cell = 0; num_cell < NUM_CELL_TOP; num_cell++)
    {
        Serial.print(F("C")); Serial.print(num_cell + 1); Serial.print(F(" UV: ")); Serial.println(uv_flag[num_ic][num_cell] );
    }

    Serial.println(F(""));

    for (uint8_t num_cell = 0; num_cell < NUM_CELL_TOP; num_cell++)
    {
        Serial.print(F("C")); Serial.print(num_cell + 1); Serial.print(F(" OV: ")); Serial.println(ov_flag[num_ic][num_cell] );
    }

    Serial.println(F(""));
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static void aux_print2user(uint8_t num_ic)
{
    Serial.println(F("[ GPIO/AUX VOLTAGE ]"));

    Serial.print(F("GPIO1 - current sense: "));  Serial.print(vgpioF[num_ic][AUX_GPIO_CURRENTSENSE], NUM_DECIMALS_GRL + 2); Serial.println(F(" V"));
    Serial.print(F("GPIO2 - select 12V/2V: "));  Serial.print(vgpioF[num_ic][AUX_GPIO_SELECT_2_12V], NUM_DECIMALS_GRL); Serial.println(F(" V"));
    Serial.print(F("GPIO3 - stack-union 24 cell: "));  Serial.print(vgpioF[num_ic][AUX_STACK_UNION], NUM_DECIMALS_GRL); Serial.println(F(" V"));
    Serial.print(F("GPIO4 - NTC10K / VREF2 buff opamp: "));  Serial.print(vgpioF[num_ic][AUX_GPIO_NTC10K], NUM_DECIMALS_GRL); Serial.println(F(" V"));
    Serial.print(F("GPIO5 - LTC6655CHMS-3.3V 0.05% 5ppm/C: "));  Serial.print(vgpioF[num_ic][AUX_GPIO_VREF_EXT], NUM_DECIMALS_GRL); Serial.println(F(" V"));
    Serial.print(F("VREF2(2.985V-3.015V): "));  Serial.print(vgpioF[num_ic][AUX_LTC_VREF2], NUM_DECIMALS_GRL); Serial.println(F(" V"));

    Serial.println(F(""));
}
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static void monitor_stack_current(void)
{
    for (uint8_t num_ic = 0; num_ic < NUM_IC; num_ic++)
    {stack_overCurrent[num_ic] = current_over_permissible_limit(stack_currentF[num_ic], stack_temperatureF[num_ic]);}
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static void monitor_stack_current_print2user(uint8_t num_ic)
{
    Serial.println(F("[ STACK CURRENT ]"));

    Serial.print(F("stack DC-current : "));  Serial.print(stack_currentF[num_ic], NUM_DECIMALS_GRL); Serial.println(F(" A"));

    Serial.print(F("over permissible limit (0=OK 1=BAD): ")); Serial.println(stack_overCurrent[num_ic]);
    Serial.println(F(""));
}
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static void monitor_stack_temperature(void)
{   //RESERVED
}
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
static void monitor_stack_temperature_print2user(uint8_t num_ic)
{
    Serial.println(F("[ STACK TEMPERATURE ]"));

    Serial.print(F("stack temperature: ")); Serial.print(stack_temperatureF[num_ic]); Serial.println(F(" C"));
    Serial.println(F(""));
}
/////////////////////////////////////////////////////////////////////////////////////
//Only import upper-stack
/////////////////////////////////////////////////////////////////////////////////////
static void stack_union_print2user(uint8_t num_ic)
{
    Serial.println(F("[ STACK-UNION ]"));

    if (num_ic % 2 != 0)
    {
        Serial.print(stack_union[num_ic]); Serial.print(F(": "));

        (stack_union[num_ic] == 1) ? Serial.print(F("lower stack + upper stack")) : Serial.println(F("unconnected stacks"));
    }
    else
    {
        Serial.println(F("lower stack don't care"));
    }
    Serial.println(F(""));
}

