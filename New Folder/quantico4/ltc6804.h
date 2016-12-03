#ifndef LTC6804_H_
#define LTC6804_H_

    #define FACTDIV 10000.0
    #define NUM_DECIMALS_2V 3
    #define NUM_DECIMALS_12V 2
    #define NUM_DECIMALS_GRL 3 //for general use

    #define AUX_GPIO_CURRENTSENSE 0
    #define AUX_GPIO_SELECT_2_12V 1
    #define AUX_STACK_UNION 2
    #define AUX_GPIO_NTC10K 3
    #define AUX_GPIO_VREF_EXT 4
    #define AUX_LTC_VREF2 5

    #define LTC_CONNECTED 1
    #define LTC_DISCONNECTED 0

    //Controls if Discharging transitors are enabled  or disabled during Cell conversions.
    #define DCP_DISABLED 0
    #define DCP_ENABLED 1

    #define PULLDOWN_CURRENT 0
    #define PULLUP_CURRENT 1

    extern float   vcellF[NUM_IC][LTC_NUM_CELLS];
    extern float   vgpioF[NUM_IC][NUM_TOTAL_REGAUX];
    extern uint8_t ltc_connected[NUM_IC];
    extern uint8_t stack_union[NUM_IC];
    extern uint8_t select_2_12V[NUM_IC];
    extern float   stack_temperatureF[NUM_IC];
    extern float   stack_voltage_ifcF[NUM_IC];//ifc=included factor corr.
    extern float   stack_currentF[NUM_IC];
    extern uint8_t uv_flag[NUM_IC][LTC_NUM_CELLS];
    extern uint8_t ov_flag[NUM_IC][LTC_NUM_CELLS];
    extern uint8_t openwire_flag[NUM_IC][LTC_NUM_CELLS];
    extern uint8_t stack_overCurrent[NUM_IC];
    extern uint8_t stack_overTemperature[NUM_IC];
    extern float   internal_digital_powerF[NUM_IC];//digital power supply, nominal:[2.7V-3.6V]
    extern float   internal_analog_powerF[NUM_IC];//analog power supply, nominal: [4.5V-5.5V]
    extern float   internal_die_tempF[NUM_IC];
    extern float   internal_socF[NUM_IC];//SOC

    extern float ov_2v;
    extern float uv_2v;
    extern float ov_12v;
    extern float uv_12v;


    void ltc6804_init(void);
    uint8_t ltc6804_job(void);

    struct _ltc6804_app
    {
        struct
        {
            struct
            {
                unsigned data_invalid:1;
                unsigned __a:7;
            }synch;
        }app;
    };
    extern struct _ltc6804_app ltc6804;

#endif // LTC6804_H_
