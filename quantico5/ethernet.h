#ifndef ETHERNET_H_
#define ETHERNET_H_

//uint8_t MAC_String2HexArray(String setting_value, uint8_t *MAC);
#include <Ethernet.h>
extern EthernetClient client;
void eth_SPI_access(void);

void eth_init(void);
void eth_scheduler_uploadData_job(void);
uint8_t eth_uploadData_job(void);
//void eth_rxDataFromServer(void);
uint8_t eth_rxDataFromServer(void);

//#define ETH_DATASEND_SETFREQ_ALWAYS 0
//#define ETH_DATASEND_SETFREQ_STOP 1
//#define ETH_DATASEND_SETFREQ_ONCE_AND_POP 2
//void eth_datasend_stackPush(uint8_t idx);
//void eth_datasend_stackPop(int8_t count=1);
//uint8_t eth_datasend_stackGetLength(void);
//void eth_datasend_stackReset(void);
//void eth_datasend_setFreq(uint8_t freq, uint8_t pop=0);


/////
#define ETH_APP_SENDDATA_REQUEST_MAX_IDX 30//Vector IDX
struct _eth_app
{
    struct _send
    {
        struct _bf
        {
            unsigned request:1;
            unsigned __a:7;
        }bf;

        uint8_t v[ETH_APP_SENDDATA_REQUEST_MAX_IDX];

    }send;

    struct _stack
    {
        int8_t sp;//deep max 127
    } stack;
};
extern struct _eth_app eth_app;

void eth_app_datasend_stackPush(uint8_t idx);
void eth_app_datasend_stackPop(int8_t count=1);
uint8_t eth_app_datasend_stackGetLength(void);
void eth_app_datasend_stackReset(void);

//////

struct _eth_con
{
    struct _bf
    {
       unsigned link:1;
       unsigned __a:7;
    }bf;

    uint8_t count_retry;
};
extern struct _eth_con eth_con;

uint8_t eth_manage_DHCP_IP(void);



enum idx_v2eth_server
{
    idx_date_str,
    idx_time_str,
    idx_ltc_connected,
    idx_select_2_12V,
    idx_stack_union,
    idx_vcellF1,
    idx_vcellF2,
    idx_vcellF3,
    idx_vcellF4,
    //idx_vgpioF,//NO RELEVANTE
    //    idx_vgpioF1,
    //    idx_vgpioF2,
    //    idx_vgpioF3,
    //    idx_vgpioF4,
    idx_uv_flag,
    //    idx_uv_flag1,
    //    idx_uv_flag2,
    //    idx_uv_flag3,
    //    idx_uv_flag4,
    //
    idx_ov_flag,
    //    idx_ov_flag1,
    //    idx_ov_flag2,
    //    idx_ov_flag3,
    //    idx_ov_flag4,
    //
    idx_openwire_flag,
    //    idx_openwire_flag1,
    //    idx_openwire_flag2,
    //    idx_openwire_flag3,
    //    idx_openwire_flag4,
    //
    idx_stack_voltage_ifcF,
    idx_stack_currentF,
    idx_stack_temperatureF,
    //idx_internal_die_tempF,//NO RELEVANTE
    //idx_internal_socF,//NO RELEVANTE
    idx_analog_input,
    idx_digital_input,
    idx_out_leds,
    idx_do_rele,
    idx_fuelsensor_value,
    //
    idx_mainvar_ApiKeyW,
    idx_mainvar_SerialNumber,
    idx_sdcardVar_PartNumber,
    idx_sdcardVar_SoftwareVersion,
    idx_mainvar_FunctionMode,
    idx_vector_dimm,
    idx_fuelsensor_model,
    idx_fuelsensor_outputtype_type,
    idx_fuelsensor_outputtype_length_units,
    idx_fuelsensor_outputtype_volume_units,
    idx_fuelsensor_tank_type,
    idx_fuelsensor_tank_rectangular_area_length,
    idx_fuelsensor_tank_rectangular_area_width,
    idx_fuelsensor_tank_irregular_spline_node_units_length_units,
    idx_fuelsensor_tank_irregular_spline_node_units_volume_units,
    idx_fuelsensor_calib_tank_innertank,
    idx_fuelsensor_calib_tank_zero2full,
    idx_fuelsensor_calib_tank_full2upper,
    //
    idx_sdcardVar_FACTCORR_2V,
    idx_sdcardVar_FACTCORR_12V_C1,
    idx_sdcardVar_FACTCORR_12V_C2,
    idx_sdcardVar_FACTCORR_12V_C3,
    idx_sdcardVar_FACTCORR_12V_C4,

    idx_sdcardVar_shuntVoltFullScale,
    idx_sdcardVar_shuntRatedCurrent,


    idx_sdcardVar_uv_2v,
    idx_sdcardVar_ov_2v,
    idx_sdcardVar_uv_12v_S3C,
    idx_sdcardVar_ov_12v_S3C,
};

#endif // ETHERNET_H_


