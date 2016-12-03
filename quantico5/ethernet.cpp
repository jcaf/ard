#include <Ethernet.h>
#include <EthernetDHCP.h>
#include "utility/w5100.h"
#include "utility/socket.h"
#include <SPI.h>
#include "system.h"

#include "ethernet.h"

#include "lcd_fxaux.h"
#include "LTC68042.h"

#include "boot.h"
#include "ltc6804.h"
#include "rtcc.h"
#include "analog_input.h"
#include "digital_input.h"
#include "leds.h"
#include "rele.h"
#include "fuelsensor.h"

//uint8_t eth_mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };//from SDCARD
uint8_t eth_server[4];// = { 192,168,0,35 };
const char eth_filePath[] PROGMEM ="/drafts/jsondecode_nuevo.php";
#define ETH_FILEPATH_LENGTH sizeof(eth_filePath)/sizeof(eth_filePath[0])
EthernetClient client;

void eth_datasend_stackPush(uint8_t idx);
void eth_datasend_stackPop(int8_t count=1);
uint8_t eth_datasend_stackGetLength(void);
void eth_datasend_stackReset(void);
void eth_datasend_setFreq(uint8_t freq, uint8_t pop=0);
#define ETH_DATASEND_SETFREQ_ALWAYS 0
#define ETH_DATASEND_SETFREQ_STOP 1
#define ETH_DATASEND_SETFREQ_ONCE_AND_POP 2

static uint16_t eth_get_totalDataLength(uint8_t total_registros);
static uint16_t eth_get_length_register(uint8_t idx);
static void eth_send_postdata(uint8_t total_registros);
static void eth_send_postdata_register(uint8_t idx);

uint8_t MAC_String2HexArray(String setting_value, uint8_t *MAC);
void IP_String2int(String serverIP, uint8_t *IP);

struct _eth_con eth_con;

const char* ip_to_str(const uint8_t* ipAddr)
{
    static char buf[16];
    sprintf(buf, "%d.%d.%d.%d\0", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    return buf;
}
/*String DisplayAddress(IPAddress address)
{
    return String(address[0]) + "." +String(address[1]) + "." +String(address[2]) + "." +String(address[3]);
}*/

void eth_SPI_access(void)
{
    SPI.endTransaction();
    SPI_deselectAllperipherals();
    SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    //SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    //SPI.begin();
}

uint8_t eth_manage_DHCP_IP(void)
{
    eth_SPI_access();

    uint8_t codret=0;

    static DhcpState prevState = DhcpStateNone;

    DhcpState state = EthernetDHCP.poll();

    if (prevState != state)
    {
        Serial.println();

        switch (state)
        {
        case DhcpStateDiscovering:
            sysPrint_serialPrintlnPSTR(PSTR("Discovering DHCP server"));
            break;
        case DhcpStateRequesting:
            sysPrint_serialPrintlnPSTR(PSTR("Requesting lease"));
            break;
        case DhcpStateRenewing:
            sysPrint_serialPrintlnPSTR(PSTR("Renewing lease"));
            break;
        case DhcpStateLeased:
            sysPrint_serialPrintlnPSTR(PSTR("Obtained lease!"));

            const byte* ipAddr = EthernetDHCP.ipAddress();
            const byte* gatewayAddr = EthernetDHCP.gatewayIpAddress();
            const byte* dnsAddr = EthernetDHCP.dnsIpAddress();

            sysPrint_serialPrintPSTR(PSTR("My IP address is "));
            sysPrint_serialPrintln(ip_to_str(ipAddr));

            sysPrint_serialPrintPSTR(PSTR("Gateway IP address is "));
            sysPrint_serialPrintln(ip_to_str(gatewayAddr));

            sysPrint_serialPrintPSTR(PSTR("DNS IP address is "));
            sysPrint_serialPrintln(ip_to_str(dnsAddr));

            eth_con.bf.link = 1;

            codret = 1;
            break;
        }

        prevState = state;
    }

    return codret;
}

void eth_init(void)
{
    unsigned long last_millis;

    //uint8_t IP[4];
    IP_String2int(mainvar_getvalue("Server1IP"), eth_server);

    sysPrint_serialPrintlnPSTR("");

    lcd.clear();
    sysPrint_lcd_serialPrintlnPSTR(1,STR_CENTER,PSTR("Set up network"));
    delay(SDCARD_BOOT_LCD_TIME2VIEW);

    //
    eth_SPI_access();
    uint8_t eth_mac[6];
    MAC_String2HexArray(mainvar_getvalue("MAC"), eth_mac);
    EthernetDHCP.begin(eth_mac, 1);//poll
    eth_con.bf.link = 0;
    //
    sysPrint_serialPrintlnPSTR("");

    //lcd.clear();
    sysPrint_lcd_serialPrintlnPSTR(0,STR_CENTER,PSTR("IP ADDRESS"));
    sysPrint_lcd_serialPrintlnPSTR(1,STR_CENTER,PSTR("Please wait while"));
    sysPrint_lcd_serialPrintlnPSTR(2,STR_CENTER,PSTR("acquiring an IP"));
    sysPrint_lcd_serialPrintlnPSTR(3,STR_CENTER,PSTR("address..."));


#define ETH_INIT_DHCP_IP_WAIT_TIME_MAX 10000// 10s
    last_millis = millis();
    while ( ( millis() - last_millis ) <  ETH_INIT_DHCP_IP_WAIT_TIME_MAX)
    {
        eth_manage_DHCP_IP();
        if  (eth_con.bf.link == 1)
            break;
    }

    lcd.clear();

    if (eth_con.bf.link == 1)
    {
        const byte* ipAddr = EthernetDHCP.ipAddress();
        const byte* gatewayAddr = EthernetDHCP.gatewayIpAddress();
        const byte* dnsAddr = EthernetDHCP.dnsIpAddress();


        sysPrint_lcdPrintPSTR(1,STR_CENTER,PSTR("My IP address:"));
        sysPrint_lcdPrint(2,STR_CENTER,String(ip_to_str(ipAddr)));

        delay(SDCARD_BOOT_LCD_TIME2VIEW);
        lcd.clear();

        sysPrint_lcdPrintPSTR(1,STR_CENTER,PSTR("Gateway IP address:"));
        sysPrint_lcdPrint(2,STR_CENTER,String(ip_to_str(gatewayAddr)));

        delay(SDCARD_BOOT_LCD_TIME2VIEW);
        lcd.clear();

        sysPrint_lcdPrintPSTR(1,STR_CENTER,PSTR("DNS IP address:"));
        sysPrint_lcdPrint(2,STR_CENTER,String(ip_to_str(dnsAddr)));

        delay(SDCARD_BOOT_LCD_TIME2VIEW);

    }
    else
    {
        sysPrint_serialPrintlnPSTR("");
        sysPrint_lcd_serialPrintlnPSTR(2,STR_CENTER,PSTR("Fail set up!"));
        delay(SDCARD_BOOT_LCD_TIME2VIEW);

    }
    lcd.clear();

    //------------
    eth_app_datasend_stackPush(idx_sdcardVar_PartNumber);
    eth_app_datasend_stackPush(idx_sdcardVar_SoftwareVersion);
    eth_app_datasend_stackPush(idx_mainvar_FunctionMode);
    eth_app_datasend_stackPush(idx_vector_dimm);
    eth_app_datasend_stackPush(idx_fuelsensor_model);
    eth_app_datasend_stackPush(idx_fuelsensor_outputtype_type);
    eth_app_datasend_stackPush(idx_fuelsensor_outputtype_length_units);
    eth_app_datasend_stackPush(idx_fuelsensor_outputtype_volume_units);
    eth_app_datasend_stackPush(idx_fuelsensor_tank_type);
    eth_app_datasend_stackPush(idx_fuelsensor_tank_rectangular_area_length);
    eth_app_datasend_stackPush(idx_fuelsensor_tank_rectangular_area_width);
    eth_app_datasend_stackPush(idx_fuelsensor_tank_irregular_spline_node_units_length_units);
    eth_app_datasend_stackPush(idx_fuelsensor_tank_irregular_spline_node_units_volume_units);
    eth_app_datasend_stackPush(idx_fuelsensor_calib_tank_innertank);
    eth_app_datasend_stackPush(idx_fuelsensor_calib_tank_zero2full);
    eth_app_datasend_stackPush(idx_fuelsensor_calib_tank_full2upper);
    eth_app_datasend_stackPush(idx_sdcardVar_FACTCORR_2V);
    eth_app_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C1);
    eth_app_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C2);
    eth_app_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C3);
    eth_app_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C4);
    eth_app_datasend_stackPush(idx_sdcardVar_shuntVoltFullScale);
    eth_app_datasend_stackPush(idx_sdcardVar_shuntRatedCurrent);
    eth_app_datasend_stackPush(idx_sdcardVar_uv_2v);
    eth_app_datasend_stackPush(idx_sdcardVar_ov_2v);
    eth_app_datasend_stackPush(idx_sdcardVar_uv_12v_S3C);
    eth_app_datasend_stackPush(idx_sdcardVar_ov_12v_S3C);

    eth_app.send.bf.request = 1;
    //------------
}


uint8_t MAC_String2HexArray(String setting_value, uint8_t *MAC)//< uint8_t MAC[6]
{
#define arr_leng 40
#define buff_length 10
#define _buff_clear() do{for (size_t i=0;i<buff_length;i++){buff[i]=0;} }while(0)

    uint8_t parseok=0;
    char arr[arr_leng];
    char buff[buff_length];
    uint8_t ca,cb,cm;
    //uint8_t MAC[6];
    ca=cb=cm=0x00;

    setting_value.toCharArray(arr,setting_value.length()+1);
    _buff_clear();

    while (arr[ca] != '\0')
    {
        if (arr[ca]==',')
        {
            MAC[cm++] =  (uint8_t) strtoul(buff, NULL, 16);
            ca++;//1++ for skip comma
            _buff_clear();
            cb=0;
        }
        buff[cb++]=arr[ca++];
    }
    MAC[5] = (uint8_t) strtoul(buff, NULL, 16);

    parseok = 1;

    return parseok;
//    for (int i=0; i<6; i++) Serial.println(MAC[i]);
}

void IP_String2int(String serverIP, uint8_t *IP)//< uint8_t IP[4]
{
    #define arr_leng 40
    #define buff_length 10
    #define _buff_clear() do{for (size_t i=0;i<buff_length;i++){buff[i]=0;} }while(0)

    char arr[arr_leng];
    char buff[buff_length];
    uint8_t ca,cb,cm;
    ca=cb=cm=0x00;

    serverIP.toCharArray(arr,serverIP.length()+1);
    _buff_clear();

    while (arr[ca] != '\0')
    {
        if (arr[ca]=='.')
        {
            IP[cm++] =  String(buff).toInt();
            ca++;//1++ for skip comma
            _buff_clear();
            cb=0;
        }
        buff[cb++]=arr[ca++];
    }
    IP[3] = String(buff).toInt();

//    for (int i=0; i<4; i++)
//    {
//        Serial.println(IP[i]);
//    }

}

//
#define VECTOR_DIMM_LENGTH 10
uint8_t vector_dimm[VECTOR_DIMM_LENGTH]=
{
    NUM_BOARD,
    NUM_IC,
    LTC_NUM_CELLS,
    NUM_CELL_2V_MAX,
    NUM_CELL_12V_MAX,
    NUM_TOTAL_REGAUX,
    NUM_ANALOG_INPUT_MAX,
    NUM_DIG_INPUT_MAX,
    NUM_LEDS_MAX,
    NUM_RELE_MAX,
};

#define LENGTH_TO_RESOLVE -1


const struct _v2eth_server
{
    const uint8_t idx;
    const char name[4];//m99 \0
    const void *pointer;
    const int16_t dim[2];//fil,col
    const uint8_t element_size;

    const char aux0[30];
}
v2eth_server[] PROGMEM=
{

    {idx_date_str, "d1",(char *)date_str,{DATE_STR_LENGTH-1,0}, sizeof(date_str[0]),""},
    {idx_time_str, "d2",(char *)time_str,{TIME_STR_LENGTH-1,0}, sizeof(time_str[0]),""},
    {idx_ltc_connected, "d3",(uint8_t *)ltc_connected,{NUM_IC,0}, sizeof(ltc_connected[0]),""},
    {idx_select_2_12V, "d4",(uint8_t *)select_2_12V,{NUM_IC,0}, sizeof(select_2_12V[0]),""},
    {idx_stack_union, "d5",(uint8_t *)stack_union,{NUM_IC,0}, sizeof(stack_union[0]),""},

    {idx_vcellF1, "d6",(float *)&vcellF[0][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(vcellF[0][0]),""},
    {idx_vcellF2, "d7",(float *)&vcellF[2][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(vcellF[2][0]),""},
    {idx_vcellF3, "d8",(float *)&vcellF[4][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(vcellF[4][0]),""},
    {idx_vcellF4, "d9",(float *)&vcellF[6][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(vcellF[6][0]),""},

//{idx_vgpioF, "d10",(float *)&vgpioF[0][0],{NUM_IC,NUM_TOTAL_REGAUX}, sizeof(vgpioF[0][0]),""},
//    {idx_vgpioF1, "d10",(float *)&vgpioF[0][0],{NUM_STACKS_IN_BOARD,NUM_TOTAL_REGAUX}, sizeof(vgpioF[0][0]),""},
//    {idx_vgpioF2, "d11",(float *)&vgpioF[2][0],{NUM_STACKS_IN_BOARD,NUM_TOTAL_REGAUX}, sizeof(vgpioF[2][0]),""},
//    {idx_vgpioF3, "d12",(float *)&vgpioF[4][0],{NUM_STACKS_IN_BOARD,NUM_TOTAL_REGAUX}, sizeof(vgpioF[4][0]),""},
//    {idx_vgpioF4, "d13",(float *)&vgpioF[6][0],{NUM_STACKS_IN_BOARD,NUM_TOTAL_REGAUX}, sizeof(vgpioF[6][0]),""},


    {idx_uv_flag, "d10",(uint8_t *)&uv_flag[0][0],{NUM_IC,LTC_NUM_CELLS}, sizeof(uv_flag[0][0]),""},
//    {idx_uv_flag1, "d10",(uint8_t *)&uv_flag[0][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(uv_flag[0][0]),""},
//    {idx_uv_flag2, "d11",(uint8_t *)&uv_flag[2][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(uv_flag[2][0]),""},
//    {idx_uv_flag3, "d12",(uint8_t *)&uv_flag[4][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(uv_flag[4][0]),""},
//    {idx_uv_flag4, "d13",(uint8_t *)&uv_flag[6][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(uv_flag[6][0]),""},
//
//
    {idx_ov_flag, "d11",(uint8_t *)&ov_flag[0][0],{NUM_IC,LTC_NUM_CELLS}, sizeof(ov_flag[0][0]),""},
//    {idx_ov_flag1, "d14",(uint8_t *)&ov_flag[0][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(ov_flag[0][0]),""},
//    {idx_ov_flag2, "d15",(uint8_t *)&ov_flag[2][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(ov_flag[2][0]),""},
//    {idx_ov_flag3, "d16",(uint8_t *)&ov_flag[4][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(ov_flag[4][0]),""},
//    {idx_ov_flag4, "d17",(uint8_t *)&ov_flag[6][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(ov_flag[6][0]),""},
//
    {idx_openwire_flag, "d12",(uint8_t *)&openwire_flag[0][0],{NUM_IC,LTC_NUM_CELLS}, sizeof(openwire_flag[0][0]),""},
//    {idx_openwire_flag1, "d18",(uint8_t *)&openwire_flag[0][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(openwire_flag[0][0]),""},
//    {idx_openwire_flag2, "d19",(uint8_t *)&openwire_flag[2][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(openwire_flag[2][0]),""},
//    {idx_openwire_flag3, "d20",(uint8_t *)&openwire_flag[4][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(openwire_flag[4][0]),""},
//    {idx_openwire_flag4, "d21",(uint8_t *)&openwire_flag[6][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(openwire_flag[6][0]),""},
//
//

    {idx_stack_voltage_ifcF, "d13",(float *)stack_voltage_ifcF,{NUM_IC,0}, sizeof(stack_voltage_ifcF[0]),""},
    {idx_stack_currentF, "d14",(float *)stack_currentF,{NUM_IC,0}, sizeof(stack_currentF[0]),""},
    {idx_stack_temperatureF, "d15",(float *)stack_temperatureF,{NUM_IC,0}, sizeof(stack_temperatureF[0]),""},
//    {idx_internal_die_tempF, "d16",(float *)internal_die_tempF,{NUM_IC,0}, sizeof(internal_die_tempF[0]),""},
//    {idx_internal_socF, "d17",(float *)internal_socF,{NUM_IC,0}, sizeof(internal_socF[0]),""},

    {idx_analog_input, "d16",(float *)analog_input,{NUM_ANALOG_INPUT_MAX,0}, sizeof(analog_input[0]),""},
    {idx_digital_input, "d17",(uint8_t *)digital_input,{NUM_DIG_INPUT_MAX,0}, sizeof(digital_input[0]),""},
    {idx_out_leds, "d18",(uint8_t *)out_leds,{NUM_LEDS_MAX,0}, sizeof(out_leds[0]),""},
    {idx_do_rele, "d19",(uint8_t *)do_rele,{NUM_RELE_MAX,0}, sizeof(do_rele[0]),""},
    {idx_fuelsensor_value, "d20",(float *)&fuelsensor.value,{0,0}, sizeof(float),""},
    //
    {idx_mainvar_ApiKeyW, "c1",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"ApiKeyW"  },
    {idx_mainvar_SerialNumber, "c2",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"SerialNumber"  },
    {idx_sdcardVar_PartNumber, "c3",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"PartNumber"  },
    {idx_sdcardVar_SoftwareVersion, "c4",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"SoftwareVersion"  },
    {idx_mainvar_FunctionMode, "c5",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"FunctionMode" },

    {idx_vector_dimm, "c6",(uint8_t *)vector_dimm,{VECTOR_DIMM_LENGTH,0}, sizeof(vector_dimm[0]),""},

    {idx_fuelsensor_model, "c7",(uint8_t *)&fuelsensor.fsEE.model,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_outputtype_type, "c8",(uint8_t*)&fuelsensor.fsEE.outputType.type,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_outputtype_length_units, "c9",(uint8_t *)&fuelsensor.fsEE.outputType.length.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_outputtype_volume_units, "c10",(uint8_t *)&fuelsensor.fsEE.outputType.volume.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_tank_type, "c11",(uint8_t *)&fuelsensor.fsEE.tank.type,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_tank_rectangular_area_length, "c12",(float *)&fuelsensor.fsEE.tank.rectangular.area.length,{0,0}, sizeof(float),""},
    {idx_fuelsensor_tank_rectangular_area_width, "c13",(float *)&fuelsensor.fsEE.tank.rectangular.area.width,{0,0}, sizeof(float),""},
    {idx_fuelsensor_tank_irregular_spline_node_units_length_units, "c14",(uint8_t *)&fuelsensor.fsEE.tank.irregular.spline.node_units.length.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_tank_irregular_spline_node_units_volume_units, "c15",(uint8_t *)&fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_calib_tank_innertank, "c16",(float *)&fuelsensor.fsEE.calib.tank.innertank,{0,0}, sizeof(float),""},
    {idx_fuelsensor_calib_tank_zero2full, "c17",(float *)&fuelsensor.fsEE.calib.tank.zero2full,{0,0}, sizeof(float),""},
    {idx_fuelsensor_calib_tank_full2upper, "c18",(float *)&fuelsensor.fsEE.calib.tank.full2upper,{0,0}, sizeof(float),""},


    {idx_sdcardVar_FACTCORR_2V, "c19",(float *)&FACTCORR_2V,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_FACTCORR_12V_C1, "c20",(float *)&FACTCORR_12V_C1,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_FACTCORR_12V_C2, "c21",(float *)&FACTCORR_12V_C2,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_FACTCORR_12V_C3, "c22",(float *)&FACTCORR_12V_C3,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_FACTCORR_12V_C4, "c23",(float *)&FACTCORR_12V_C4,{0,0}, sizeof(float),""  },

    {idx_sdcardVar_shuntVoltFullScale, "c24",(float *)&shuntVoltFullScale,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_shuntRatedCurrent, "c25",(float *)&shuntRatedCurrent,{0,0}, sizeof(float),""  },

    {idx_sdcardVar_uv_2v, "c26",(float *)&uv_2v,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_ov_2v, "c27",(float *)&ov_2v,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_uv_12v_S3C, "c28",(float *)&uv_12v_S3C,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_ov_12v_S3C, "c29",(float *)&ov_12v_S3C,{0,0}, sizeof(float),""  },

};

#define NUM_VECTOR2SERVER sizeof(v2eth_server)/sizeof(v2eth_server[0])

uint8_t eth_datasend[NUM_VECTOR2SERVER];

struct _eth
{
    struct _datasend
    {
        uint8_t frequency;
        uint8_t number_of_pops;

    } datasend;

    struct _stack
    {
        int8_t sp;//deep max 127
    } stack;
} eth;


void eth_datasend_stackPush(uint8_t idx)
{
    eth_datasend[eth.stack.sp] = idx;

    if (++eth.stack.sp > NUM_VECTOR2SERVER)
        eth.stack.sp = 0;
}

void eth_datasend_stackPop(int8_t count)//int8_t count=1
{
    for (int i = 0; i < count; ++i)
    {
        if (--eth.stack.sp < 0)
            eth.stack.sp = NUM_VECTOR2SERVER;
    }
}
uint8_t eth_datasend_stackGetLength(void)
{
    return (uint8_t)eth.stack.sp;
}
void eth_datasend_stackReset(void)//reset sp
{
    eth.stack.sp = 0;
}


void eth_datasend_setFreq(uint8_t freq, uint8_t pop)//reset sp
{
    eth.datasend.frequency = freq;
    eth.datasend.number_of_pops = pop;//valid only for ETH_DATASEND_SETFREQ_ONCE_AND_POP
}
uint8_t eth_datasend_getFreq(void)
{
    return eth.datasend.frequency;
}

void eth_datasend_parsingAfterSend(void)
{
    if (eth_datasend_getFreq() == ETH_DATASEND_SETFREQ_ONCE_AND_POP)
    {
        eth_datasend_stackPop(eth.datasend.number_of_pops);
    }

    if (eth_datasend_stackGetLength()==0)
    {
        eth_datasend_setFreq(ETH_DATASEND_SETFREQ_STOP);
    }
    else
    {
        eth_datasend_setFreq(ETH_DATASEND_SETFREQ_ALWAYS); //x defect
    }

}
//////////////////////////////////////////////////////////////////////
struct _eth_app eth_app;

uint8_t eth_app_datasend_getData(uint8_t idx)
{
    return eth_app.send.v[idx];
}

void eth_app_datasend_stackPush(uint8_t idx)
{
    eth_app.send.v[eth_app.stack.sp] = idx;

    if (++eth_app.stack.sp > ETH_APP_SENDDATA_REQUEST_MAX_IDX)
        eth_app.stack.sp = 0;
}

void eth_app_datasend_stackPop(int8_t count)//int8_t count=1
{
    for (int i = 0; i < count; ++i)
    {
        if (--eth_app.stack.sp < 0)
            eth_app.stack.sp = ETH_APP_SENDDATA_REQUEST_MAX_IDX;
    }
}
uint8_t eth_app_datasend_stackGetLength(void)
{
    return (uint8_t)eth_app.stack.sp;
}
void eth_app_datasend_stackReset(void)//reset sp
{
    eth_app.stack.sp = 0;
}

//////////////////////////////////////////////////////////////////////
void eth_scheduler_uploadData_job(void)
{
    static uint8_t sm0;

    if (eth_con.bf.link == 1)
    {
        eth_datasend_stackReset();

        switch (sm0)
        {
        case 0:
            eth_datasend_stackPush(idx_date_str);
            eth_datasend_stackPush(idx_time_str);
            eth_datasend_stackPush(idx_ltc_connected);
            eth_datasend_stackPush(idx_select_2_12V);
            eth_datasend_stackPush(idx_stack_union);

            sm0++;
            break;

        case 1:
            eth_datasend_stackPush(idx_vcellF1);
            sm0++;
            break;

        case 2:
            eth_datasend_stackPush(idx_vcellF2);
            sm0++;
            break;

        case 3:
            eth_datasend_stackPush(idx_vcellF3);
            sm0++;
            break;

        case 4:
            eth_datasend_stackPush(idx_vcellF4);
            sm0++;
            break;

        case 5:
            eth_datasend_stackPush(idx_uv_flag);
            sm0++;
            break;

        case 6:
            eth_datasend_stackPush(idx_ov_flag);
            sm0++;
            break;

        case 7:
            eth_datasend_stackPush(idx_openwire_flag);
            sm0++;
            break;

        case 8:
            eth_datasend_stackPush(idx_stack_voltage_ifcF);
            eth_datasend_stackPush(idx_stack_currentF);
            eth_datasend_stackPush(idx_stack_temperatureF);

            sm0++;
            break;

        case 9:
            eth_datasend_stackPush(idx_analog_input);
            eth_datasend_stackPush(idx_digital_input);
            eth_datasend_stackPush(idx_out_leds);
            eth_datasend_stackPush(idx_do_rele);
            eth_datasend_stackPush(idx_fuelsensor_value);

            sm0=0;
            break;

        }
        //
        eth_datasend_stackPush(idx_mainvar_ApiKeyW);
        eth_datasend_stackPush(idx_mainvar_SerialNumber);


        if (eth_app.send.bf.request)
        {
            eth_app.send.bf.request = 0;

            uint8_t app_datasend_stackLength =  eth_app_datasend_stackGetLength();

            for (uint8_t i=0; i<app_datasend_stackLength; i++ )
            {
                eth_datasend_stackPush( eth_app_datasend_getData(i) );
            }
            eth_datasend_setFreq(ETH_DATASEND_SETFREQ_ONCE_AND_POP, app_datasend_stackLength  );

            eth_app_datasend_stackReset();
        }

        eth_uploadData_job();
    }
}

//void eth_rxDataFromServer(void);
void ShowSocketStatus();

uint8_t uno=1;

uint8_t eth_uploadData_job(void)
{
    char filePath[ETH_FILEPATH_LENGTH];

    eth_SPI_access();


    int8_t eth_data2send_length =  eth_datasend_stackGetLength();

    if (eth_data2send_length > 0)
    {
        if (client.connect(eth_server, 80))
        {
            strcpy_P(filePath, (char *) &eth_filePath);

            client.print(F("POST "));

            //client.print(filePath);
            client.print(mainvar_getvalue("Directory"));

            client.println(F(" HTTP/1.1"));


            client.print(F("Host: "));
            client.println(mainvar_getvalue("Host"));//githubprototypes.battery.com"));//obligado for HTTP 1.1.


            client.println(F("Content-Type: application/json"));//client.println("Content-Type: application/x-www-form-urlencoded; charset=utf-8");
            client.println(F("Connection: close"));
            client.println(F("User-Agent: FIRWAR/1.00"));
            //
            client.print(F("Content-Length: "));
            client.println(eth_get_totalDataLength(eth_data2send_length));

            client.println();
            //
            eth_send_postdata(eth_data2send_length);
            eth_datasend_parsingAfterSend();//add
            //
            //eth_rxDataFromServer();

        }
        else
        {
            sysPrint_serialPrintlnPSTR(PSTR("Conexion failed!"));

            client.stop();
            eth_con.bf.link = 0;


            //eth_SPI_access();
            uint8_t eth_mac[6];
            MAC_String2HexArray(mainvar_getvalue("MAC"), eth_mac);
            EthernetDHCP.begin(eth_mac, 1);//poll
        }

    }

//   ShowSocketStatus();
}
/*
uint8_t eth_rxDataFromServer(void)
{
    uint8_t cod_ret=0;
    static uint8_t sm0 = 0;
    static unsigned long last_millis;

    if (eth_con.bf.link == 1)
    {
        if (sm0 == 0)
        {
            last_millis = millis();
            sm0++;
        }
        if (sm0 == 1)
        {
            if ( (millis() - last_millis) < 7000)  //eth_server answer: end of rx = '?'
            {
                if (client.connected() )
                {
                    if (client.available())
                    {
                        char rx= client.read();
                        Serial.print(rx);
                        if (rx == '?')
                        {
                            client.stop();
                            client.flush();
                            sysPrint_serialPrintlnPSTR(PSTR("client.stop"));
                            //
                            sm0 =0;
                            cod_ret = 1;
                        }
                    }

                }
            }
            else
            {
                client.stop();
                client.flush();
                sysPrint_serialPrintlnPSTR(PSTR("client.stop"));

                sm0 =0;
                cod_ret = 1;//finish
            }
        }
    }
    return cod_ret;
}
*/

uint8_t eth_rxDataFromServer(void)
{
    uint8_t cod_ret=0;
    //static uint8_t sm0 = 0;
    //static
    unsigned long last_millis;

    if (eth_con.bf.link == 1)
    {
        last_millis = millis();

        while (client.connected())
        {
            if  ( (millis() - last_millis) > 10000)//2000)
            {
                break;
            }
            else
            {
                if (client.available())
                {
                    char rx= client.read();
                    Serial.print(rx);
                    if (rx == '?')
                    {
                        client.stop();
                        client.flush();
                        sysPrint_serialPrintlnPSTR(PSTR("client.stop"));

                        break;
                        //
                        //sm0 =0;
                        //cod_ret = 1;
                    }
                }

            }
        }


        client.stop();
        client.flush();
        sysPrint_serialPrintlnPSTR(PSTR("client.stop"));

        //sm0 =0;
        //cod_ret = 1;//finish

    }

    return cod_ret;
}



static uint16_t eth_get_totalDataLength(uint8_t total_registros)
{
    uint16_t acc=0;
    if (total_registros > 0)
    {
        for (uint8_t v=0; v < total_registros; v++)
        {
            acc+=eth_get_length_register(eth_datasend[v]);

            if (v < (total_registros-1))
                acc+=1;// comma between vectors
        }
        acc+=2; // open{ }close
    }
    return acc;
}
//#define V2SERVER_GETDATALENGTH_DEBUG
static uint16_t eth_get_length_register(uint8_t idx)
{
    //uint8_t idx;
    int16_t dimx, dimy;
    uint8_t element_size;
    char name[4];
    void *pointer;
    char buf[15];
    char aux0[30];
    uint16_t acc=0;
    uint16_t acc1;
    //
    uint8_t fil;
    uint8_t col;

    //1.-
    //idx       = pgm_read_byte_near((const uint8_t *) &v2eth_server[idx].idx);
    strcpy_P(name, (const char *) &v2eth_server[idx].name);
    dimx    = pgm_read_word_near((const int16_t *) &v2eth_server[idx].dim[0]);
    dimy    = pgm_read_word_near((const int16_t *) &v2eth_server[idx].dim[1]);
    element_size = pgm_read_byte_near((const uint8_t *) &v2eth_server[idx].element_size);

    if (dimx == LENGTH_TO_RESOLVE )
    {
        strcpy_P(aux0, (char *) &v2eth_server[idx].aux0);

        //--------
        char setting_value[SDCARD_SETTING_VALUE_MAX_LENGTH];

        if (strcmp(aux0,"PartNumber")== 0)
        {
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("PartNumber"),aux0) )
            {
                //Serial.println(String(aux0));
                dimx= strlen(aux0);
            }
        }
        else if (strcmp(aux0,"SoftwareVersion")== 0)
        {
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("SoftwareVersion"),aux0) )
            {
                dimx= strlen(aux0);
                //Serial.println(String(aux0));
            }
        }
        //--------
        else
        {
            String t = mainvar_getvalue(String(aux0));

            dimx    = t.length();
            t.toCharArray(aux0, dimx+1);

            //pointer = (void *)aux0;
            //dimx    = strlen(aux0);
        }

    }
    else
    {
        pointer = (void *) pgm_read_word(&(v2eth_server[idx].pointer));
    }


#ifdef V2SERVER_GETDATALENGTH_DEBUG
    Serial.println(idx);
    Serial.println(dimx);
    Serial.println(dimy);
    Serial.println(element_size);
#endif

    //2.-
    //# of bytes containing inside vectors
    if ((dimx>0) || (dimy>0))
    {
        if ((idx==idx_ltc_connected)||(idx==idx_stack_union)||
                (idx==idx_digital_input)||(idx==idx_out_leds)||(idx==idx_do_rele))
        {
            acc += dimx;
        }
        else if ( (idx==idx_uv_flag)||(idx==idx_ov_flag)||(idx==idx_openwire_flag) )
        {
            acc += (dimx*dimy);
        }
        else if (idx==idx_time_str)
        {
            acc += dimx;//time_str;
        }
        else if (idx==idx_date_str)
        {
            acc += dimx;//date_str
        }
        else if (
            (idx==idx_mainvar_ApiKeyW) || (idx==idx_mainvar_SerialNumber) || (idx==idx_mainvar_FunctionMode)
            ||(idx==idx_sdcardVar_PartNumber)
            ||(idx==idx_sdcardVar_SoftwareVersion)

        )
        {
            acc += strlen(aux0);
        }
        else
        {
            //-------
            if ((dimx>0) && (dimy==0))//1 array
            {
                fil = 1;
                col = dimx;
            }
            else
            {
                fil = dimx;
                col = dimy;
            }
            //-------

            for (uint8_t f=0; f<fil; f++)
            {
                for (uint8_t c=0; c<col; c++)
                {
                    if ((idx==idx_stack_voltage_ifcF)||(idx==idx_stack_currentF)||(idx==idx_stack_temperatureF)
                            //||(idx==idx_vcellF)
                            //||(idx==idx_vgpioF)
                            //||(idx==idx_internal_die_tempF)
                            //||(idx==idx_internal_socF)
                            || (idx==idx_analog_input)

                            //++
                            || (idx == idx_vcellF1)
                            || (idx == idx_vcellF2)
                            || (idx == idx_vcellF3)
                            || (idx == idx_vcellF4)
                       )
                    {
                        dtostrf(*(float *)pointer,4,3,buf);//acc += sprintf(buf, "%.3f", *(float *)pointer);
                        acc+= strlen(buf);

#ifdef V2SERVER_GETDATALENGTH_DEBUG
                        Serial.print(*(float *)pointer,3);
                        Serial.print(" ");
#endif
                    }
                    else if ((idx==idx_select_2_12V) ||(idx==idx_vector_dimm))
                    {
                        itoa(*(uint8_t *)pointer, buf, 10);//acc += sprintf(buf, "%u", *(uint8_t *)pointer);
                        acc+= strlen(buf);
                    }

                    pointer = (uint8_t *)pointer + element_size;        //(uint8_t *) for arithmetic between "bytes"
                }
#ifdef V2SERVER_GETDATALENGTH_DEBUG
                Serial.println();
#endif
            }
        }
    }
    else
    {
        //variables
        if (
            (idx==idx_fuelsensor_value)
            ||(idx==idx_fuelsensor_tank_rectangular_area_length)||(idx==idx_fuelsensor_tank_rectangular_area_width)
            ||(idx==idx_fuelsensor_calib_tank_innertank)||(idx==idx_fuelsensor_calib_tank_zero2full)||(idx==idx_fuelsensor_calib_tank_full2upper)



            ||(idx==idx_sdcardVar_FACTCORR_12V_C1)
            ||(idx==idx_sdcardVar_FACTCORR_12V_C2)
            ||(idx==idx_sdcardVar_FACTCORR_12V_C3)
            ||(idx==idx_sdcardVar_FACTCORR_12V_C4)

             ||(idx==idx_sdcardVar_shuntVoltFullScale)
            ||(idx==idx_sdcardVar_shuntRatedCurrent)

            ||(idx==idx_sdcardVar_uv_2v)
            ||(idx==idx_sdcardVar_ov_2v)
            ||(idx==idx_sdcardVar_uv_12v_S3C)
            ||(idx==idx_sdcardVar_ov_12v_S3C)

        )
        {
            dtostrf(*(float *)pointer,4,3,buf);//3 decimals
            acc+= strlen(buf);
        }
        else if (idx==idx_sdcardVar_FACTCORR_2V)
        {
            dtostrf(*(float *)pointer,4,4,buf);//4 decimals
            acc+= strlen(buf);
        }
        else if (
            (idx==idx_fuelsensor_model)
            ||(idx==idx_fuelsensor_outputtype_type)||(idx==idx_fuelsensor_outputtype_length_units)||(idx==idx_fuelsensor_outputtype_volume_units)
            ||(idx==idx_fuelsensor_tank_type)
            ||(idx==idx_fuelsensor_tank_irregular_spline_node_units_length_units)||(idx==idx_fuelsensor_tank_irregular_spline_node_units_volume_units)
        )
        {
            acc+= 1;
        }

    }
    //printf("num elementos %i \n",acc);

    ////////////////////////////////////
    /*
    [
    ["","","",""],
    ["","","",""],
    ["","","",""]
    ]

    [""]
    */
    //fil=col=1;

    acc1=0;

    if ((dimx>0) && (dimy==0))//1 array
    {
        fil = 1;
        col = dimx;
    }
    else if ((dimx==0) && (dimy==0))
    {
        fil= 1;
        col= 1;
    }
    else
    {
        fil = dimx;
        col = dimy;
    }
    //cuento numero de comas y demas dentro del valor del registro
    acc1+= (col-1);    //# comas
    acc1+= (col*2);    //# ""
    acc1+= 2;        //# brackets internos
    acc1*=fil;
    acc1+= (fil-1);  //# de comas entre registros
    if (dimy>0)
        acc1+= 2;    //brackets externos
    ////////////////////////////////////
    //cuento el nombre del registro
    acc1+=3; //"mxX":
    acc1+= strlen(name);//mxx


    ////////////////////////////////////
    acc+= acc1;//add to acc

#ifdef V2SERVER_GETDATALENGTH_DEBUG
    Serial.println("acc is");//printf("acc is %i \n", acc);
    Serial.println( acc);
#endif


#ifdef V2SERVER_GETDATALENGTH_DEBUG
    Serial.println();
    Serial.println( acc);
#endif
    return acc;
}

static void eth_send_postdata(uint8_t total_registros)
{
    if (total_registros > 0)
    {
        client.print(F("{"));
        for (uint8_t v=0; v < total_registros; v++)
        {
            eth_send_postdata_register(eth_datasend[v]);

            if (v < (total_registros - 1))
                client.print(F(","));
        }
        client.println(F("}"));
    }
}

static void eth_send_postdata_register(uint8_t idx)
{
    //uint8_t idx
    int16_t dimx, dimy;
    uint8_t element_size;
    char name[4];
    void *pointer;
    uint8_t fil;
    uint8_t col;
    char aux0[30];

    //1.- Set values
    //idx = pgm_read_byte_near((const uint8_t *) &v2eth_server[v].idx);
    strcpy_P(name, (const char *) &v2eth_server[idx].name);
    dimx    = pgm_read_word_near((const int16_t *) &v2eth_server[idx].dim[0]);
    dimy    = pgm_read_word_near((const int16_t *) &v2eth_server[idx].dim[1]);
    element_size = pgm_read_byte_near((const uint8_t *) &v2eth_server[idx].element_size);

    if (dimx == LENGTH_TO_RESOLVE )
    {
        strcpy_P(aux0, (const char *) &v2eth_server[idx].aux0);

        //--------
        //char setting_value[SDCARD_SETTING_VALUE_MAX_LENGTH];
        if (strcmp(aux0,"PartNumber")== 0)
        {
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("PartNumber"),aux0) )
            {
            }
        }
        else if (strcmp(aux0,"SoftwareVersion")== 0)
        {
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("SoftwareVersion"),aux0) )
            {
            }
        }
        //--------
        else
        {
            mainvar_getvalue(aux0).toCharArray(aux0, 30);
        }

        pointer = (void *)aux0;
        dimx    = strlen(aux0);
    }
    else
    {
        pointer = (void *) pgm_read_word(&(v2eth_server[idx].pointer));
    }

    //2.- Check/Interchange dimensions
    if ((dimx > 0) && (dimy == 0))//1 array
    {
        fil = 1;
        col = dimx;
    }
    else if ((dimx == 0) && (dimy == 0))
    {
        fil = 1;
        col = 1;
    }
    else
    {
        fil = dimx;
        col = dimy;
    }

    //3.- Print data
    //-----------------------------------
    client.print(F("\""));
    client.print(name);
    client.print(F("\":"));
    //-----------------------------------
    if ((dimy > 0) && (dimx > 0))//array bidimensional
        client.print(F("["));
    //-----------------------------------
    for (uint8_t f = 0; f < fil; f++)
    {
        client.print(F("["));
        for (uint8_t c = 0; c < col; c++)
        {
            client.print(F("\""));

            //++-print data
            if ((idx == idx_vector_dimm) || (idx == idx_ltc_connected) || (idx == idx_uv_flag) || (idx == idx_ov_flag) || (idx == idx_openwire_flag) || (idx == idx_stack_union) || (idx == idx_select_2_12V)
                    || (idx == idx_digital_input) || (idx == idx_out_leds) || (idx == idx_do_rele)
                    || (idx == idx_fuelsensor_model)
                    || (idx == idx_fuelsensor_outputtype_type) || (idx == idx_fuelsensor_outputtype_length_units) || (idx == idx_fuelsensor_outputtype_volume_units)
                    || (idx == idx_fuelsensor_tank_type)
                    || (idx == idx_fuelsensor_tank_irregular_spline_node_units_length_units) || (idx == idx_fuelsensor_tank_irregular_spline_node_units_volume_units)
               )
            {
                client.print(*(uint8_t *) pointer);
            }
            else if ((idx == idx_stack_voltage_ifcF) || (idx == idx_stack_currentF) || (idx == idx_stack_temperatureF)
                     //|| (idx == idx_vcellF)
                     //|| (idx == idx_vgpioF)
                     //|| (idx == idx_internal_die_tempF)
                     //|| (idx == idx_internal_socF)
                     || (idx == idx_analog_input)
                     || (idx == idx_fuelsensor_value)
                     || (idx == idx_fuelsensor_tank_rectangular_area_length) || (idx == idx_fuelsensor_tank_rectangular_area_width)
                     || (idx == idx_fuelsensor_calib_tank_innertank) || (idx == idx_fuelsensor_calib_tank_zero2full) || (idx == idx_fuelsensor_calib_tank_full2upper)

                     ||(idx==idx_sdcardVar_FACTCORR_12V_C1)
                     ||(idx==idx_sdcardVar_FACTCORR_12V_C2)
                     ||(idx==idx_sdcardVar_FACTCORR_12V_C3)
                     ||(idx==idx_sdcardVar_FACTCORR_12V_C4)

                     ||(idx==idx_sdcardVar_shuntVoltFullScale)
                     ||(idx==idx_sdcardVar_shuntRatedCurrent)

                     ||(idx==idx_sdcardVar_uv_2v)
                     ||(idx==idx_sdcardVar_ov_2v)
                     ||(idx==idx_sdcardVar_uv_12v_S3C)
                     ||(idx==idx_sdcardVar_ov_12v_S3C)

                     //++
                     || (idx == idx_vcellF1)
                     || (idx == idx_vcellF2)
                     || (idx == idx_vcellF3)
                     || (idx == idx_vcellF4)

                    )
            {
                client.print(*(float *) pointer, 3);//3 decimals
            }
            else if (idx==idx_sdcardVar_FACTCORR_2V)
            {
                client.print(*(float *) pointer, 4);//4 decimals
            }
            else if (
                (idx == idx_time_str) || (idx == idx_date_str)
                || (idx == idx_mainvar_FunctionMode)
                || (idx == idx_mainvar_ApiKeyW)
                || (idx == idx_mainvar_SerialNumber)

                ||(idx==idx_sdcardVar_PartNumber)
                ||(idx==idx_sdcardVar_SoftwareVersion)
            )
            {
                client.print(*(char *) pointer);
            }
            //--+

            client.print(F("\""));

            if (col > 1)// # elements>1
            {
                if (c < (col - 1))
                    client.print(F(",")); //more elements
            }
            pointer = (uint8_t *) pointer + element_size;
        }
        client.print(F("]"));

        if ((dimy > 0) && (dimx > 0))//array bidimensional
        {
            if (f < (fil - 1))
                client.print(F(","));
        }
    }

    //-----------------------------------
    if (dimy > 0)//array bidimensional
        client.print(F("]"));
    //-----------------------------------
}
//////////////////////////////////////////////++++++++++++++++++++++++
//-- Code: ---unsigned long G_SocketCheckTimer = 0;
//extern unsigned long G_SocketConnectionTimers[MAX_SOCK_NUM]; //system millis
//unsigned long G_SocketConnectionTimes[MAX_SOCK_NUM];  //time hh:mm:ss
void ShowSocketStatus()
{

//    ActivityWriteSPICSC("ETHERNET SOCKET LIST");
//    ActivityWriteSPICSC("#:Status Port Destination DPort");
//    ActivityWriteSPICSC("0=avail,14=waiting,17=connected,22=UDP");
//    ActivityWriteSPICSC("1C=close wait");
    String l_line = "";
    l_line.reserve(64);
    char l_buffer[10] = "";

    for (uint8_t i = 0; i < MAX_SOCK_NUM; i++)
    {
        l_line = "#" + String(i);

        uint8_t s = W5100.readSnSR(i); //status
        l_line += ":0x";
        sprintf(l_buffer,"%x",s);
        l_line += l_buffer;
//
// if(s == 0x1C)
//{close(i);}
//
        l_line += " ";
        l_line += String(W5100.readSnPORT(i)); //port

        l_line += " D:";
        uint8_t dip[4];
        W5100.readSnDIPR(i, dip); //IP Address
        for (int j=0; j<4; j++)
        {
            l_line += int(dip[j]);
            if (j<3) l_line += ".";
        }
        l_line += " (";
        l_line += String(W5100.readSnDPORT(i)); //port on destination
        l_line += ") ";

//        if (G_SocketConnectionTimes[i] != 0)
//            l_line += TimeToHHMM(G_SocketConnectionTimes[i]);

        Serial.println(l_line);

        //ActivityWriteSPICSC(l_line);
    }
}

