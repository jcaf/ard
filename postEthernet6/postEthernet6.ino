#include <Ethernet.h>
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };
byte server[] = { 192,168,1,42 };
EthernetClient client;

static uint16_t eth_get_totalDataLength(uint8_t total_registros);
static uint16_t eth_get_length_register(uint8_t idx);
static void eth_send_postdata(uint8_t total_registros);
static void eth_send_postdata_register(uint8_t idx);

#define NUM_BOARD 4
#define NUM_IC NUM_BOARD*2
#define LTC_NUM_CELLS 12
#define NUM_CELL_2V_MAX LTC_NUM_CELLS
#define NUM_CELL_12V_MAX 4
#define NUM_TOTAL_REGAUX (5+1) //5 GPIO + 1 VREF2
#define NUM_ANALOG_INPUT_MAX 4
#define NUM_DIG_INPUT_MAX 4
#define NUM_LEDS_MAX 6
#define NUM_RELE_MAX 5
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
#define TIME_STR_LENGTH 12
#define DATE_STR_LENGTH 9
char date_str[DATE_STR_LENGTH]="08/06/16";//dd/mm/yy0
char time_str[TIME_STR_LENGTH]="12:31:28 AM";//hh:mm:ss am0
uint8_t ltc_connected[NUM_IC]= {1,0,1,0,1,0,1,0};
uint8_t select_2_12V[NUM_IC]= {2,2,2,2,12,12,12,12};
uint8_t stack_union[NUM_IC]= {0,1,0,1,0,1,0,1};
float   vcellF[NUM_IC][LTC_NUM_CELLS];
float   stack_voltage_ifcF[NUM_IC]= {12.123, 11.123, 10.123, 9.123, 12.123, 11.123, 10.123, 9.123}; //ifc=included factor corr.
float   stack_currentF[NUM_IC]={1.123, 2.123, 3.123, 4.123, 5.123, 6.123, 7.123, 8.123};
float   stack_temperatureF[NUM_IC]={11.123, 22.123, 33.123, 44.123, 55.123, 66.123, 77.123, 88.123};
uint8_t uv_flag[NUM_IC][LTC_NUM_CELLS];
uint8_t ov_flag[NUM_IC][LTC_NUM_CELLS];
uint8_t openwire_flag[NUM_IC][LTC_NUM_CELLS];
float   vgpioF[NUM_IC][NUM_TOTAL_REGAUX];
float   internal_die_tempF[NUM_IC];
float   internal_socF[NUM_IC];//SOC
//
float analog_input[NUM_ANALOG_INPUT_MAX];
uint8_t digital_input[NUM_DIG_INPUT_MAX];
uint8_t out_leds[NUM_LEDS_MAX];
uint8_t do_rele[NUM_RELE_MAX];

//////////////////////////////////////////////////////
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

#define SPLINE_NODES_MIN 3
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
                    } length;

                    struct _volume
                    {
                        uint8_t units;//gallons/liters
                    } volume;

                } node_units;

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
struct _fsEEPROM fsEEPROM;
/*
struct _spline_polyvector
{
    float A[SPLINE_NODES_MAX];
    float B[SPLINE_NODES_MAX];
    float C[SPLINE_NODES_MAX];
    float D[SPLINE_NODES_MAX];
};
*/

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

struct _fuelsensor fuelsensor;

////////////////////////////////////////////////////////////////////////////
#define cfgfiles_VARIABLES_LENGTH_MAX 30//50
struct _mainvar
{
    const char setting_name[cfgfiles_VARIABLES_LENGTH_MAX];
    String setting_value;
    uint8_t match_pair;

} mainvar[]=
{
    //funcmod.txt
    {"FunctionMode","Production"},

    //setcon.txt
    {"MAC","0x90,0xA2,0xDA,0x0E,0x08,0x51"},
    {"Server1IP","192.168.1.45"},
    {"Host","quanticoservices.com"},
    {"Directory","monitor-v01-01"},
    {"ApiKeyW","2S25AHPLERPTOKFF"},

    //specs.txt
    {"SerialNumber","40773206"},

    //ADD++
    {"PartNumber","MB-2.0.0-15"},
    {"SoftwareVersion","kernel-2.0.0-15"},

    //setup.txt
    //nothing to do
    
    //corfact.txt
    {"FACTCORR_2V","+0.0042"},
    {"FACTCORR_12V_C1","+0.073"},
    {"FACTCORR_12V_C2","+0.064"},
    {"FACTCORR_12V_C3","+0.047"},
    {"FACTCORR_12V_C4","+0.067"},

    //setacq.txt
    {"shuntMultiplier","1000"},
    {"uv_2v","1.8"},
    {"ov_2v","2.2"},
    {"uv_12v_S3C","11.7"},
    {"ov_12v_S3C","12.5"},
};

#define NUM_MAINVARS sizeof(mainvar)/sizeof(mainvar[0])

/********************************************//**
 * @brief Found main variable
 *
 * @param setting_name String; is the "search key"
 * @return int8_t; -1 if var not found, else, return the index >=0
 *
 ***********************************************/
int8_t mainvar_isFound(String setting_name)
{
    char setting_name_buff[cfgfiles_VARIABLES_LENGTH_MAX];
    int8_t index = -1;

    for (size_t i=0; i<NUM_MAINVARS; i++)
    {
        setting_name.toCharArray(setting_name_buff, setting_name.length()+1 );

        if ( strcmp(mainvar[i].setting_name, setting_name_buff ) == 0 )
        {
            index = i;
            break;
        }
    }
    return index;
}
/********************************************//**
 * @brief Main variable get value (global struct mainvar)
 *          find trough the setting name
 * @param setting_name String; is the "search key"
 * @return String; return the string get value if found, else return ""
 *
 ***********************************************/
String mainvar_getvalue(String setting_name)
{
    String setting_value="";

    int8_t index = mainvar_isFound(setting_name);
    if (index > -1)
        setting_value = mainvar[(size_t)index].setting_value;

    return setting_value;
}
/////////////////////////////////////////////////////////////////////////////


enum idx_v2server{
    idx_date_str,
    idx_time_str,
    idx_ltc_connected,
    idx_select_2_12V,
    idx_stack_union,
    idx_vcellF,
    idx_stack_voltage_ifcF,
    idx_stack_currentF,
    idx_stack_temperatureF,
    idx_uv_flag,
    idx_ov_flag,
    idx_openwire_flag,
    idx_vgpioF,
    idx_internal_die_tempF,
    idx_internal_socF,
    idx_analog_input,
    idx_digital_input,
    idx_out_leds,
    idx_do_rele,
    idx_fuelsensor_value,
    //
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
    idx_mainvar_FunctionMode,
    idx_mainvar_ApiKeyW,
    idx_mainvar_SerialNumber,

    idx_sdcardVar_PartNumber,
    idx_sdcardVar_SoftwareVersion,

    idx_sdcardVar_FACTCORR_2V,
    idx_sdcardVar_FACTCORR_12V_C1,
    idx_sdcardVar_FACTCORR_12V_C2,
    idx_sdcardVar_FACTCORR_12V_C3,
    idx_sdcardVar_FACTCORR_12V_C4,

    idx_sdcardVar_shuntMultiplier,
    idx_sdcardVar_uv_2v,
    idx_sdcardVar_ov_2v,
    idx_sdcardVar_uv_12v_S3C,
    idx_sdcardVar_ov_12v_S3C,
};

#define LENGTH_TO_RESOLVE -1

void init_data(void);
void random_data(void);

float FACTCORR_2V;// = +0.0042;// + 0.0202 LTC genera una caida de tension
float FACTCORR_12V_C1;// = +0.073;
float FACTCORR_12V_C2;// = +0.064;
float FACTCORR_12V_C3;// = +0.047;
float FACTCORR_12V_C4;// = +0.067;
float ov_2v;// = 3.500 + FACTCORR_2V;//<= 5.73V
float uv_2v;// = 1.000 + FACTCORR_2V;//>= 0.0016V
float ov_12v_S3C;// = 13.500;//S3C Sum of 3 Cell
float uv_12v_S3C;// = 10.0;//1.950;

void setup()
{
    Serial.begin(230400);//9600);
    init_data();//simulating system

    /*
    while (1)
    {
        random_data();
        for (uint8_t i=0; i<NUM_IC ; i++ )
        {
          for (uint8_t y=0; y<LTC_NUM_CELLS; y++ )
            {

              Serial.print("vc:" + String(vcellF[i][y]) + String(" "));
              Serial.print("uv:" + String(uv_flag[i][y]) + String(" "));
              Serial.print("ov:" + String(ov_flag[i][y]) + String(" "));
            }

            Serial.println();
            Serial.println("stack_voltage_ifcF:" + String(stack_voltage_ifcF[i]) + String(" "));
            Serial.println();
        }

        delay(2);
    }
    */
    eth_init();
}

void loop()
{
    random_data();
    
    eth_uploadData_job();
    while (client.connected())
    {
        if (client.available())
        {
              char rx= client.read();
              Serial.print(rx);
              if (rx == '?')//end of connection
              {
                client.stop();
              }
        }
     }
  //delay(120000);//2 min
  delay(10000);
}

uint8_t eth_init(void)
{
    if(!Ethernet.begin(mac))
    {
        Serial.println("Could not configure Ethernet with DHCP");
    }
    else
    {
        Serial.println(Ethernet.localIP());
    }

}

/*
uint8_t ethernet_init(void)
{
    uint8_t coderet=0;
    uint8_t MAC[6];

    lcd.clear();
    sysPrint_lcd_serialPrintln(1,STR_CENTER,"INTERNET CONNECTION");
    delay(2000);
    lcd.clear();
    sysPrint_lcd_serialPrintln(0,STR_CENTER,"ACQUIRING IP ADDRESS");

    MAC_String2HexArray(mainvar_getvalue("MAC"), MAC);

    if(!Ethernet.begin(MAC))
    {
        sysPrint_lcd_serialPrintln(2,0,"ERROR: NO IP with DHCP");
    }
    else
    {
        coderet = 1;
        sysPrint_lcd_serialPrintln(1,STR_CENTER,"Ethernet Configured!");
        sysPrint_lcd_serialPrintln(2,STR_CENTER,"IP address is at:");
        //Ethernet.localIP()

    }
    delay(2000);

    return coderet;
}
*/

const struct _v2server
{
    const uint8_t idx;
    const char name[4];//m99 \0
    const void *pointer;
    const int16_t dim[2];//fil,col
    const uint8_t element_size;

    const char aux0[30];
}
v2server[] PROGMEM=
{
    {idx_date_str, "d1",(char *)date_str,{DATE_STR_LENGTH-1,0}, sizeof(date_str[0]),""},
    {idx_time_str, "d2",(char *)time_str,{TIME_STR_LENGTH-1,0}, sizeof(time_str[0]),""},
    {idx_ltc_connected, "d3",(uint8_t *)ltc_connected,{NUM_IC,0}, sizeof(ltc_connected[0]),""},
    {idx_select_2_12V, "d4",(uint8_t *)select_2_12V,{NUM_IC,0}, sizeof(select_2_12V[0]),""},
    {idx_stack_union, "d5",(uint8_t *)stack_union,{NUM_IC,0}, sizeof(stack_union[0]),""},
    {idx_vcellF, "d6",(float *)vcellF,{NUM_IC,LTC_NUM_CELLS}, sizeof(vcellF[0][0]),""},
    {idx_stack_voltage_ifcF, "d7",(float *)stack_voltage_ifcF,{NUM_IC,0}, sizeof(stack_voltage_ifcF[0]),""},
    {idx_stack_currentF, "d8",(float *)stack_currentF,{NUM_IC,0}, sizeof(stack_currentF[0]),""},
    {idx_stack_temperatureF, "d9",(float *)stack_temperatureF,{NUM_IC,0}, sizeof(stack_temperatureF[0]),""},
    {idx_uv_flag, "d10",(uint8_t *)uv_flag,{NUM_IC,LTC_NUM_CELLS}, sizeof(uv_flag[0][0]),""},
    {idx_ov_flag, "d11",(uint8_t *)ov_flag,{NUM_IC,LTC_NUM_CELLS}, sizeof(ov_flag[0][0]),""},
    {idx_openwire_flag, "d12",(uint8_t *)openwire_flag,{NUM_IC,LTC_NUM_CELLS}, sizeof(openwire_flag[0][0]),""},
    {idx_vgpioF, "d13",(float *)vgpioF,{NUM_IC,NUM_TOTAL_REGAUX}, sizeof(vgpioF[0][0]),""},
    {idx_internal_die_tempF, "d14",(float *)internal_die_tempF,{NUM_IC,0}, sizeof(internal_die_tempF[0]),""},
    {idx_internal_socF, "d15",(float *)internal_socF,{NUM_IC,0}, sizeof(internal_socF[0]),""},
    {idx_analog_input, "d16",(float *)analog_input,{NUM_ANALOG_INPUT_MAX,0}, sizeof(analog_input[0]),""},
    {idx_digital_input, "d17",(uint8_t *)digital_input,{NUM_DIG_INPUT_MAX,0}, sizeof(digital_input[0]),""},
    {idx_out_leds, "d18",(uint8_t *)out_leds,{NUM_LEDS_MAX,0}, sizeof(out_leds[0]),""},
    {idx_do_rele, "d19",(uint8_t *)do_rele,{NUM_RELE_MAX,0}, sizeof(do_rele[0]),""},
    {idx_fuelsensor_value, "d20",(float *)&fuelsensor.value,{0,0}, sizeof(float),""},
    //
    {idx_vector_dimm, "c1",(uint8_t *)vector_dimm,{VECTOR_DIMM_LENGTH,0}, sizeof(vector_dimm[0]),""},
    {idx_fuelsensor_model, "c2",(uint8_t *)&fsEEPROM.model,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_outputtype_type, "c3",(uint8_t*)&fsEEPROM.outputType.type,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_outputtype_length_units, "c4",(uint8_t *)&fsEEPROM.outputType.length.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_outputtype_volume_units, "c5",(uint8_t *)&fsEEPROM.outputType.volume.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_tank_type, "c6",(uint8_t *)&fsEEPROM.tank.type,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_tank_rectangular_area_length, "c7",(float *)&fsEEPROM.tank.rectangular.area.length,{0,0}, sizeof(float),""},
    {idx_fuelsensor_tank_rectangular_area_width, "c8",(float *)&fsEEPROM.tank.rectangular.area.width,{0,0}, sizeof(float),""},
    {idx_fuelsensor_tank_irregular_spline_node_units_length_units, "c9",(uint8_t *)&fsEEPROM.tank.irregular.spline.node_units.length.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_tank_irregular_spline_node_units_volume_units, "c10",(uint8_t *)&fsEEPROM.tank.irregular.spline.node_units.volume.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_calib_tank_innertank, "c11",(float *)&fsEEPROM.calib.tank.innertank,{0,0}, sizeof(float),""},
    {idx_fuelsensor_calib_tank_zero2full, "c12",(float *)&fsEEPROM.calib.tank.zero2full,{0,0}, sizeof(float),""},
    {idx_fuelsensor_calib_tank_full2upper, "c13",(float *)&fsEEPROM.calib.tank.full2upper,{0,0}, sizeof(float),""},

    {idx_mainvar_FunctionMode, "c14",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char) ,"FunctionMode" },
    {idx_mainvar_ApiKeyW, "c15",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"ApiKeyW"  },
    {idx_mainvar_SerialNumber, "c16",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"SerialNumber"  },

    {idx_sdcardVar_PartNumber, "c17",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"PartNumber"  },
    {idx_sdcardVar_SoftwareVersion, "c18",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"SoftwareVersion"  },
    {idx_sdcardVar_FACTCORR_2V, "c19",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"FACTCORR_2V"  },
    {idx_sdcardVar_FACTCORR_12V_C1, "c20",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"FACTCORR_12V_C1"  },
    {idx_sdcardVar_FACTCORR_12V_C2, "c21",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"FACTCORR_12V_C2"  },
    {idx_sdcardVar_FACTCORR_12V_C3, "c22",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"FACTCORR_12V_C3"  },
    {idx_sdcardVar_FACTCORR_12V_C4, "c23",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"FACTCORR_12V_C4"  },
    {idx_sdcardVar_shuntMultiplier, "c24",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"shuntMultiplier"  },
    {idx_sdcardVar_uv_2v, "c25",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"uv_2v"  },
    {idx_sdcardVar_ov_2v, "c26",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"ov_2v"  },
    {idx_sdcardVar_uv_12v_S3C, "c27",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"uv_12v_S3C"  },
    {idx_sdcardVar_ov_12v_S3C, "c28",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"ov_12v_S3C"  },

};

#define NUM_VECTOR2SERVER sizeof(v2server)/sizeof(v2server[0])

uint8_t eth_datasend[NUM_VECTOR2SERVER];

struct _eth
{
    struct _datasend
    {
        uint8_t frequency;
        uint8_t number_of_pops;

    }datasend;

    struct _stack
    {
      int8_t sp;//deep max 127
    }stack;
}eth;

void eth_datasend_stackPush(uint8_t idx);
void eth_datasend_stackPop(int8_t count=1);
uint8_t eth_datasend_stackGetLength(void);
void eth_datasend_stackReset(void);
void eth_datasend_setFreq(uint8_t freq, uint8_t pop=0);

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

#define ETH_DATASEND_SETFREQ_ALWAYS 0
#define ETH_DATASEND_SETFREQ_STOP 1
#define ETH_DATASEND_SETFREQ_ONCE_AND_POP 2


void eth_datasend_setFreq(uint8_t freq, uint8_t pop)//reset sp
{
    eth.datasend.frequency = freq;
    eth.datasend.number_of_pops = pop;//valid only for ETH_DATASEND_SETFREQ_ONCE_AND_POP
}

void eth_datasend_parsingAfterSend(void)
{
    if (eth.datasend.frequency == ETH_DATASEND_SETFREQ_ONCE_AND_POP)
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

void init_data(void)
{
  char buf[10];
  String str;
 
  fuelsensor.value = 1234.567;
  //
  str = mainvar_getvalue("FACTCORR_2V");
  str.toCharArray(buf,10);
  FACTCORR_2V = strtod(buf, (char**)0);

  //
  str = mainvar_getvalue("FACTCORR_12V_C1");
  str.toCharArray(buf,10);
  FACTCORR_12V_C1 = strtod(buf, (char**)0);

  //
  str = mainvar_getvalue("FACTCORR_12V_C2");
  str.toCharArray(buf,10);
  FACTCORR_12V_C2 = strtod(buf, (char**)0);

  //
  str = mainvar_getvalue("FACTCORR_12V_C3");
  str.toCharArray(buf,10);
  FACTCORR_12V_C3 = strtod(buf, (char**)0);

  //
  str = mainvar_getvalue("FACTCORR_12V_C4");
  str.toCharArray(buf,10);
  FACTCORR_12V_C4 = strtod(buf, (char**)0);

  //
  str = mainvar_getvalue("uv_2v");
  str.toCharArray(buf,10);
  uv_2v = strtod(buf, (char**)0);
  uv_2v += FACTCORR_2V;
  //
  str = mainvar_getvalue("ov_2v");
  str.toCharArray(buf,10);
  ov_2v = strtod(buf, (char**)0);
  ov_2v += FACTCORR_2V;
   //
  str = mainvar_getvalue("uv_12v_S3C");
  str.toCharArray(buf,10);
  uv_12v_S3C = strtod(buf, (char**)0);

  //
  str = mainvar_getvalue("ov_12v_S3C");
  str.toCharArray(buf,10);
  ov_12v_S3C = strtod(buf, (char**)0);
  
  //Serial.println(FACTCORR_2V,3);
  //Serial.println(FACTCORR_12V_C1,3);
  //Serial.println(FACTCORR_12V_C2,3);
  //Serial.println(FACTCORR_12V_C3,3);
  //Serial.println(FACTCORR_12V_C4,3);
  //Serial.println(uv_2v,3);
  //Serial.println(ov_2v,3);
  //Serial.println(uv_12v_S3C,3);
  //Serial.println(ov_12v_S3C,3);
  //while (1);

    //
    eth_datasend_stackReset();
    eth_datasend_setFreq(ETH_DATASEND_SETFREQ_ALWAYS);

    eth_datasend_stackPush(idx_date_str);
    eth_datasend_stackPush(idx_time_str);
    eth_datasend_stackPush(idx_ltc_connected);
    eth_datasend_stackPush(idx_select_2_12V);
    eth_datasend_stackPush(idx_stack_union);
    eth_datasend_stackPush(idx_vcellF);
    eth_datasend_stackPush(idx_stack_voltage_ifcF);
    eth_datasend_stackPush(idx_stack_currentF);
    eth_datasend_stackPush(idx_stack_temperatureF);
    eth_datasend_stackPush(idx_uv_flag);
    eth_datasend_stackPush(idx_ov_flag);
    eth_datasend_stackPush(idx_openwire_flag);
    eth_datasend_stackPush(idx_vgpioF);
    eth_datasend_stackPush(idx_internal_die_tempF);
    eth_datasend_stackPush(idx_internal_socF);
    eth_datasend_stackPush(idx_analog_input);
    eth_datasend_stackPush(idx_digital_input);
    eth_datasend_stackPush(idx_out_leds);
    eth_datasend_stackPush(idx_do_rele);
    eth_datasend_stackPush(idx_fuelsensor_value);
    //
    eth_datasend_stackPush(idx_vector_dimm);
    eth_datasend_stackPush(idx_fuelsensor_model);
    eth_datasend_stackPush(idx_fuelsensor_outputtype_type);
    eth_datasend_stackPush(idx_fuelsensor_outputtype_length_units);
    eth_datasend_stackPush(idx_fuelsensor_outputtype_volume_units);
    eth_datasend_stackPush(idx_fuelsensor_tank_type);
    eth_datasend_stackPush(idx_fuelsensor_tank_rectangular_area_length);
    eth_datasend_stackPush(idx_fuelsensor_tank_rectangular_area_width);
    eth_datasend_stackPush(idx_fuelsensor_tank_irregular_spline_node_units_length_units);
    eth_datasend_stackPush(idx_fuelsensor_tank_irregular_spline_node_units_volume_units);
    eth_datasend_stackPush(idx_fuelsensor_calib_tank_innertank);
    eth_datasend_stackPush(idx_fuelsensor_calib_tank_zero2full);
    eth_datasend_stackPush(idx_fuelsensor_calib_tank_full2upper);
    eth_datasend_stackPush(idx_mainvar_FunctionMode);
    eth_datasend_stackPush(idx_mainvar_ApiKeyW);
    eth_datasend_stackPush(idx_mainvar_SerialNumber);

    eth_datasend_stackPush(idx_sdcardVar_PartNumber);
    eth_datasend_stackPush(idx_sdcardVar_SoftwareVersion);

    eth_datasend_stackPush(idx_sdcardVar_FACTCORR_2V);
    eth_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C1);
    eth_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C2);
    eth_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C3);
    eth_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C4);

    eth_datasend_stackPush(idx_sdcardVar_shuntMultiplier);
    eth_datasend_stackPush(idx_sdcardVar_uv_2v);
    eth_datasend_stackPush(idx_sdcardVar_ov_2v);
    eth_datasend_stackPush(idx_sdcardVar_uv_12v_S3C);
    eth_datasend_stackPush(idx_sdcardVar_ov_12v_S3C);
}

uint8_t eth_uploadData_job(void)
{
  int8_t eth_data2send_length =  eth_datasend_stackGetLength();

  if (eth_data2send_length > 0)
  {
    if (client.connect(server, 80))
    {
        client.println(F("POST /jsondecode.php HTTP/1.1"));
        client.println(F("Host: 192.168.1.42"));//obligado for HTTP 1.1.
        //client.println(F("Host: www.firwar.com"));//obligado for HTTP 1.1.
        client.println(F("Content-Type: application/json"));//client.println("Content-Type: application/x-www-form-urlencoded; charset=utf-8");
        client.println(F("Connection: close"));
        client.println(F("User-Agent: FIRWAR/1.00"));
        //
        client.print(F("Content-Length: "));
        client.println(eth_get_totalDataLength(eth_data2send_length));
        client.println();
        //
        eth_send_postdata(eth_data2send_length);
    }
  }
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
    char buf[12];
    char aux0[30];
    uint16_t acc=0;
    uint16_t acc1;
    //
    uint8_t fil;
    uint8_t col;

    //1.-
    //idx       = pgm_read_byte_near((const uint8_t *) &v2server[idx].idx);
    strcpy_P(name, (const char *) &v2server[idx].name);
    dimx    = pgm_read_word_near((const int16_t *) &v2server[idx].dim[0]);
    dimy    = pgm_read_word_near((const int16_t *) &v2server[idx].dim[1]);
    element_size = pgm_read_byte_near((const uint8_t *) &v2server[idx].element_size);

    if (dimx == LENGTH_TO_RESOLVE )
    {
      strcpy_P(aux0, (const char *) &v2server[idx].aux0);
            mainvar_getvalue(aux0).toCharArray(aux0, 30);

            pointer = (void *)aux0;
            dimx    = strlen(aux0);
    }
    else
    {
        pointer = (void *) pgm_read_word(&(v2server[idx].pointer));
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

          ||(idx==idx_sdcardVar_FACTCORR_2V)
          ||(idx==idx_sdcardVar_FACTCORR_12V_C1)
          ||(idx==idx_sdcardVar_FACTCORR_12V_C2)
          ||(idx==idx_sdcardVar_FACTCORR_12V_C3)
          ||(idx==idx_sdcardVar_FACTCORR_12V_C4)

          ||(idx==idx_sdcardVar_shuntMultiplier)
          ||(idx==idx_sdcardVar_uv_2v)
          ||(idx==idx_sdcardVar_ov_2v)
          ||(idx==idx_sdcardVar_uv_12v_S3C)
          ||(idx==idx_sdcardVar_ov_12v_S3C)
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
                    if ((idx==idx_stack_voltage_ifcF)||(idx==idx_stack_currentF)||(idx==idx_stack_temperatureF)||(idx==idx_vcellF)||(idx==idx_vgpioF)
                            ||(idx==idx_internal_die_tempF)||(idx==idx_internal_socF)|| (idx==idx_analog_input)
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
        )
        {
            dtostrf(*(float *)pointer,4,3,buf);
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
    //idx = pgm_read_byte_near((const uint8_t *) &v2server[v].idx);
    strcpy_P(name, (const char *) &v2server[idx].name);
    dimx    = pgm_read_word_near((const int16_t *) &v2server[idx].dim[0]);
    dimy    = pgm_read_word_near((const int16_t *) &v2server[idx].dim[1]);
    element_size = pgm_read_byte_near((const uint8_t *) &v2server[idx].element_size);

    if (dimx == LENGTH_TO_RESOLVE )
    {
        strcpy_P(aux0, (const char *) &v2server[idx].aux0);
        mainvar_getvalue(aux0).toCharArray(aux0, 30);

        pointer = (void *)aux0;
        dimx    = strlen(aux0);
    }
    else
    {
        pointer = (void *) pgm_read_word(&(v2server[idx].pointer));
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
            else if ((idx == idx_stack_voltage_ifcF) || (idx == idx_stack_currentF) || (idx == idx_stack_temperatureF) || (idx == idx_vcellF) || (idx == idx_vgpioF)
                     || (idx == idx_internal_die_tempF) || (idx == idx_internal_socF) || (idx == idx_analog_input)
                     || (idx == idx_fuelsensor_value)
                     || (idx == idx_fuelsensor_tank_rectangular_area_length) || (idx == idx_fuelsensor_tank_rectangular_area_width)
                     || (idx == idx_fuelsensor_calib_tank_innertank) || (idx == idx_fuelsensor_calib_tank_zero2full) || (idx == idx_fuelsensor_calib_tank_full2upper)
                    )
            {
                client.print(*(float *) pointer, 3);
            }
            else if (
                (idx == idx_time_str) || (idx == idx_date_str)
                || (idx == idx_mainvar_FunctionMode)
                || (idx == idx_mainvar_ApiKeyW)
                || (idx == idx_mainvar_SerialNumber)

                ||(idx==idx_sdcardVar_PartNumber)
                ||(idx==idx_sdcardVar_SoftwareVersion)

                ||(idx==idx_sdcardVar_FACTCORR_2V)
                ||(idx==idx_sdcardVar_FACTCORR_12V_C1)
                ||(idx==idx_sdcardVar_FACTCORR_12V_C2)
                ||(idx==idx_sdcardVar_FACTCORR_12V_C3)
                ||(idx==idx_sdcardVar_FACTCORR_12V_C4)

                ||(idx==idx_sdcardVar_shuntMultiplier)
                ||(idx==idx_sdcardVar_uv_2v)
                ||(idx==idx_sdcardVar_ov_2v)
                ||(idx==idx_sdcardVar_uv_12v_S3C)
                ||(idx==idx_sdcardVar_ov_12v_S3C)
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

//////////////////////////////////////////// random //////////////////////////

int get_int_random(int min, int max)
{
  static uint8_t _seed;
    randomSeed(millis() + (_seed++) );
    return random(min, max);
}
float get_float_random(int min, int max)
{
    float entero = get_int_random(min, max);
    float decimal = ((float)rand()/ (float)(RAND_MAX)) * 1.0;
    return (entero + decimal );
}
/////////////////////////////////////////////////////////////////////////////////
void random_data(void)
{
    uint8_t idx;
    int16_t dimx, dimy;
    uint8_t element_size;
    char name[4];
    void *pointer;
    uint8_t fil;
    uint8_t col;
    char aux0[30];

    for (uint8_t v=0; v<NUM_VECTOR2SERVER; v++)
    {
         //1.- Set values
      idx = pgm_read_byte_near((const uint8_t *) &v2server[v].idx);
      strcpy_P(name, (const char *) &v2server[v].name);
      dimx    = pgm_read_word_near((const int16_t *) &v2server[v].dim[0]);
      dimy    = pgm_read_word_near((const int16_t *) &v2server[v].dim[1]);
      element_size = pgm_read_byte_near((const uint8_t *) &v2server[v].element_size);

      if (dimx == LENGTH_TO_RESOLVE )
      {
          strcpy_P(aux0, (const char *) &v2server[v].aux0);
          mainvar_getvalue(aux0).toCharArray(aux0, 30);

          pointer = (void *)aux0;
          dimx    = strlen(aux0);
      }
      else
      {
          pointer = (void *) pgm_read_word(&(v2server[v].pointer));
      }

        //-----------------------------------
        //interchange of dimension
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
        //-----------------------------------
        for (uint8_t f=0; f<fil; f++)
        {
            for (uint8_t c=0; c<col ; c++)
            {
                if (idx == idx_ltc_connected)
                {
                    *((uint8_t *)pointer) = get_int_random(0,1);
                }
                else if (idx == idx_select_2_12V)
                {
                    //x ahora fijos
                }
                else if (idx == idx_stack_union)
                {
                    //x ahora fijos
                }
                
    
                else if (idx == idx_vcellF)
                {
                    if (select_2_12V[f] == 2)
                        *((float *)pointer) = get_float_random(1,3) + FACTCORR_2V;
                    else if (select_2_12V[f] == 12)
                    {
                        if (c<4)//0,1,2,3
                        {
                            float vcell = get_float_random(11,13);
                            if (c==0)
                              vcell += FACTCORR_12V_C1;
                            else if (c==1)
                              vcell += FACTCORR_12V_C2;
                            else if (c==2)
                              vcell += FACTCORR_12V_C3;
                            else if (c==3)
                              vcell += FACTCORR_12V_C4;
                            
                            *((float *)pointer) = vcell;
                        }
                        else
                        {
                            *((float *)pointer) = 0;
                        }
                    }
                }
                else if (idx == idx_stack_voltage_ifcF)
                {
                    uint8_t acc=0;
                    uint8_t count_cell;

                    if (select_2_12V[c] == 2)
                        count_cell = NUM_CELL_2V_MAX;
                    else if (select_2_12V[c] == 12)
                        count_cell = NUM_CELL_12V_MAX;

                    for (uint8_t cell=0; cell < count_cell ; cell++)
                        acc +=vcellF[c][cell];

                    *((float *)pointer) = acc;
                }
                else if (idx == idx_stack_currentF)
                {
                    *((float *)pointer) = get_float_random(300,600);
                }
                else if (idx == idx_stack_temperatureF)
                {
                    *((float *)pointer) = get_float_random(23,29);
                }
                else if (idx == idx_uv_flag)
                {
                    if (select_2_12V[f] == 2)
                    {
                      if (vcellF[f][c] < uv_2v)
                        *((uint8_t *)pointer) = 1;
                      else
                        *((uint8_t *)pointer) = 0;
                    }
                    else if (select_2_12V[f] == 12)
                    {
                        if (c<4)//0,1,2,3
                        {
                             if (vcellF[f][c] < uv_12v_S3C)
                              *((uint8_t *)pointer) = 1;
                            else
                              *((uint8_t *)pointer) = 0;
                        }
                        else
                        {
                            *((uint8_t *)pointer) = 0;
                        }
                    }
                  
                }
                else if (idx == idx_ov_flag)
                {
                   if (select_2_12V[f] == 2)
                    {
                      if (vcellF[f][c] > ov_2v)
                        *((uint8_t *)pointer) = 1;
                      else
                        *((uint8_t *)pointer) = 0;
                    }
                    else if (select_2_12V[f] == 12)
                    {
                        if (c<4)//0,1,2,3
                        {
                             if (vcellF[f][c] > ov_12v_S3C)
                              *((uint8_t *)pointer) = 1;
                            else
                              *((uint8_t *)pointer) = 0;
                        }
                        else
                        {
                            *((uint8_t *)pointer) = 0;
                        }
                    }

                }
                else if (idx == idx_openwire_flag)
                {
                    *((uint8_t *)pointer) = get_int_random(0,1);
                }



                //
                pointer = (uint8_t *)pointer + element_size;
            }

        }
    }

}


