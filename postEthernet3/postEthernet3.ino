#include <Ethernet.h>
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };
byte server[] = { 192,168,1,47 };
EthernetClient client;

void v2server_postData_json(void);

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

#define VECTOR_DIMM_LENGTH 10
uint8_t vector_dimm[VECTOR_DIMM_LENGTH]={
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
uint8_t ltc_connected[NUM_IC]= {1,0,1,0,1,0,1,0};
float   stack_voltage_ifcF[NUM_IC]= {12.123, 11.123, 10.123, 9.123, 12.123, 11.123, 10.123, 9.123}; //ifc=included factor corr.
float   stack_currentF[NUM_IC];
float   stack_temperatureF[NUM_IC];
uint8_t select_2_12V[NUM_IC];
uint8_t stack_union[NUM_IC];

float   vcellF[NUM_IC][LTC_NUM_CELLS];
uint8_t uv_flag[NUM_IC][LTC_NUM_CELLS];
uint8_t ov_flag[NUM_IC][LTC_NUM_CELLS];
uint8_t openwire_flag[NUM_IC][LTC_NUM_CELLS];
float   vgpioF[NUM_IC][NUM_TOTAL_REGAUX];

float   internal_die_tempF[NUM_IC];
float   internal_socF[NUM_IC];//SOC

float analog_input[NUM_ANALOG_INPUT_MAX];
uint8_t digital_input[NUM_DIG_INPUT_MAX];
uint8_t out_leds[NUM_LEDS_MAX];
uint8_t do_rele[NUM_RELE_MAX];

#define TIME_STR_LENGTH 12
#define DATE_STR_LENGTH 9
char time_str[TIME_STR_LENGTH]="12:31:28 AM";//hh:mm:ss am0
char date_str[DATE_STR_LENGTH]="08/06/16";//dd/mm/yy0

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

    //setup.txt

    //corfact.txt

    //setacq.txt
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

//---------------------------
#define idx_vector_dimm         0
#define idx_ltc_connected       1
#define idx_stack_voltage_ifcF  2
#define idx_stack_currentF      3
#define idx_stack_temperatureF  4
#define idx_select_2_12V        5
#define idx_stack_union         6
#define idx_vcellF              7
#define idx_uv_flag             8
#define idx_ov_flag             9
#define idx_openwire_flag       10
#define idx_vgpioF              11
#define idx_internal_die_tempF  12
#define idx_internal_socF       13
#define idx_analog_input        14
#define idx_digital_input       15
#define idx_out_leds            16
#define idx_do_rele             17
#define idx_time_str            18
#define idx_date_str            19

#define idx_fuelsensor_value 20
#define idx_fuelsensor_model 21

#define idx_fuelsensor_outputtype_type 22
#define idx_fuelsensor_outputtype_length_units 23
#define idx_fuelsensor_outputtype_volume_units 24

#define idx_fuelsensor_tank_type 25
#define idx_fuelsensor_tank_rectangular_area_length 26
#define idx_fuelsensor_tank_rectangular_area_width 27

#define idx_fuelsensor_tank_irregular_spline_node_units_length_units 28
#define idx_fuelsensor_tank_irregular_spline_node_units_volume_units 29

#define idx_fuelsensor_calib_tank_innertank 30
#define idx_fuelsensor_calib_tank_zero2full 31
#define idx_fuelsensor_calib_tank_full2upper 32

#define idx_mainvar_FunctionMode 33
#define idx_mainvar_ApiKeyW 34
#define idx_mainvar_SerialNumber 35

#define LENGTH_DYNAMIC 1

void setup()
{
    Serial.begin(230400);//9600);
    ethernet_init();

//init data
fuelsensor.value = 1234.567;

//
    //Serial.println(v2server_getDataLength() );
    //v2server_postData_json();
    ethernet_uploadData_job();
}

void loop()
{
    if (1)//(client.connected())
    {
        if (client.available())
        {
            /*
                   if (1)//(client.find((char *)"<led_1>"))
                    {
                        int led_1 = client.parseInt();
                        Serial.println(led_1);
                    }
              */
            char led_1 = client.read();
            Serial.print(led_1);
            // client.stop();
        }

    }

}

uint8_t ethernet_init(void)
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

#define NUM_VECTOR2SERVER 36
const struct _v2server
{
    const uint8_t idx;
    const void *pointer;
    const uint8_t dim[2];//fil,col
    const uint8_t element_size;
}
v2server[NUM_VECTOR2SERVER] PROGMEM=
{
    {idx_vector_dimm, (uint8_t *)vector_dimm,{VECTOR_DIMM_LENGTH,0}, sizeof(vector_dimm[0])},
    {idx_ltc_connected, (uint8_t *)ltc_connected,{NUM_IC,0}, sizeof(ltc_connected[0])},
    {idx_stack_voltage_ifcF, (float *)stack_voltage_ifcF,{NUM_IC,0}, sizeof(stack_voltage_ifcF[0])},

    {idx_stack_currentF, (float *)stack_currentF,{NUM_IC,0}, sizeof(stack_currentF[0])},
    {idx_stack_temperatureF, (float *)stack_temperatureF,{NUM_IC,0}, sizeof(stack_temperatureF[0])},
    {idx_select_2_12V, (uint8_t *)select_2_12V,{NUM_IC,0}, sizeof(select_2_12V[0])},
    {idx_stack_union, (uint8_t *)stack_union,{NUM_IC,0}, sizeof(stack_union[0])},

    {idx_vcellF, (float *)vcellF,{NUM_IC,LTC_NUM_CELLS}, sizeof(vcellF[0][0])},
    {idx_uv_flag, (uint8_t *)uv_flag,{NUM_IC,LTC_NUM_CELLS}, sizeof(uv_flag[0][0])},
    {idx_ov_flag, (uint8_t *)ov_flag,{NUM_IC,LTC_NUM_CELLS}, sizeof(ov_flag[0][0])},
    {idx_openwire_flag, (uint8_t *)openwire_flag,{NUM_IC,LTC_NUM_CELLS}, sizeof(openwire_flag[0][0])},
    {idx_vgpioF, (float *)vgpioF,{NUM_IC,NUM_TOTAL_REGAUX}, sizeof(vgpioF[0][0])},

    {idx_internal_die_tempF, (float *)internal_die_tempF,{NUM_IC,0}, sizeof(internal_die_tempF[0])},
    {idx_internal_socF, (float *)internal_socF,{NUM_IC,0}, sizeof(internal_socF[0])},

    {idx_analog_input, (float *)analog_input,{NUM_ANALOG_INPUT_MAX,0}, sizeof(analog_input[0])},
    {idx_digital_input, (uint8_t *)digital_input,{NUM_DIG_INPUT_MAX,0}, sizeof(digital_input[0])},
    {idx_out_leds, (uint8_t *)out_leds,{NUM_LEDS_MAX,0}, sizeof(out_leds[0])},
    {idx_do_rele, (uint8_t *)do_rele,{NUM_RELE_MAX,0}, sizeof(do_rele[0])},

    {idx_time_str, (char *)time_str,{TIME_STR_LENGTH-1,0}, sizeof(time_str[0])},
    {idx_date_str, (char *)date_str,{DATE_STR_LENGTH-1,0}, sizeof(date_str[0])},

    {idx_fuelsensor_value, (float *)&fuelsensor.value,{0,0}, sizeof(float)},
    {idx_fuelsensor_model, (uint8_t *)&fsEEPROM.model,{0,0}, sizeof(uint8_t)},

    {idx_fuelsensor_outputtype_type, (uint8_t*)&fsEEPROM.outputType.type,{0,0}, sizeof(uint8_t)},
    {idx_fuelsensor_outputtype_length_units, (uint8_t *)&fsEEPROM.outputType.length.units,{0,0}, sizeof(uint8_t)},
    {idx_fuelsensor_outputtype_volume_units, (uint8_t *)&fsEEPROM.outputType.volume.units,{0,0}, sizeof(uint8_t)},

    {idx_fuelsensor_tank_type, (uint8_t *)&fsEEPROM.tank.type,{0,0}, sizeof(uint8_t)},
    {idx_fuelsensor_tank_rectangular_area_length, (float *)&fsEEPROM.tank.rectangular.area.length,{0,0}, sizeof(float)},
    {idx_fuelsensor_tank_rectangular_area_width, (float *)&fsEEPROM.tank.rectangular.area.width,{0,0}, sizeof(float)},
    
    {idx_fuelsensor_tank_irregular_spline_node_units_length_units, (uint8_t *)&fsEEPROM.tank.irregular.spline.node_units.length.units,{0,0}, sizeof(uint8_t)},
    {idx_fuelsensor_tank_irregular_spline_node_units_volume_units, (uint8_t *)&fsEEPROM.tank.irregular.spline.node_units.volume.units,{0,0}, sizeof(uint8_t)},

    {idx_fuelsensor_calib_tank_innertank, (float *)&fsEEPROM.calib.tank.innertank,{0,0}, sizeof(float)},
    {idx_fuelsensor_calib_tank_zero2full, (float *)&fsEEPROM.calib.tank.zero2full,{0,0}, sizeof(float)},
    {idx_fuelsensor_calib_tank_full2upper, (float *)&fsEEPROM.calib.tank.full2upper,{0,0}, sizeof(float)},

    {idx_mainvar_FunctionMode, (void *)0x00 ,{LENGTH_DYNAMIC,0}, sizeof(char)  },
    {idx_mainvar_ApiKeyW, (void *)0x00 ,{LENGTH_DYNAMIC,0}, sizeof(char)  },
    {idx_mainvar_SerialNumber, (void *)0x00 ,{LENGTH_DYNAMIC,0}, sizeof(char)  },
};

//#define V2SERVER_GETDATALENGTH_DEBUG
uint16_t v2server_getDataLength(void)
{
    uint8_t idx,dimx,dimy,element_size;
    void *pointer;
    char buf[12];
    char buf1[50];
    uint16_t acc=0;
    uint16_t acc1;
    //
    uint8_t fil;
    uint8_t col;

    for (uint8_t v=0; v<NUM_VECTOR2SERVER ; v++ )
    {
        idx       = pgm_read_byte_near((const uint8_t *) &v2server[v].idx);
        dimx    = pgm_read_byte_near((const uint8_t *) &v2server[v].dim[0]);
        dimy    = pgm_read_byte_near((const uint8_t *) &v2server[v].dim[1]);
        element_size = pgm_read_byte_near((const uint8_t *) &v2server[v].element_size);
        //pointer = (void *) pgm_read_word(&(v2server[v].pointer));

        //if (dimx ==
        if (idx==idx_mainvar_FunctionMode)
        {
          mainvar_getvalue("FunctionMode").toCharArray(buf1, 50);
          pointer = (void *)buf1;
          dimx    = strlen(buf1);
        }
        else if (idx==idx_mainvar_ApiKeyW)
        {
          mainvar_getvalue("ApiKeyW").toCharArray(buf1, 50);
          pointer = (void *)buf1;
          dimx    = strlen(buf1);
        }
        else if (idx==idx_mainvar_SerialNumber)
        {
          mainvar_getvalue("SerialNumber").toCharArray(buf1, 50);
          pointer = (void *)buf1;
          dimx    = strlen(buf1);
        }
        else
        {
          pointer = (void *) pgm_read_word(&(v2server[v].pointer));
        }
        

#ifdef V2SERVER_GETDATALENGTH_DEBUG
        Serial.println(idx);
        Serial.println(dimx);
        Serial.println(dimy);
        Serial.println(element_size);
#endif
        
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
            else if ( (idx==idx_mainvar_ApiKeyW) || (idx==idx_mainvar_SerialNumber) || (idx==idx_mainvar_FunctionMode) )
            {
              acc += strlen(buf1);
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

        acc1+= (col-1);    //# comas
        acc1+= (col*2);    //# ""
        acc1+= 2;        //# brackets internos
        acc1*=fil;
        acc1+= (fil-1);  //# de comas entre registros
        if (dimy>0)
            acc1+= 2;    //brackets externos
        ////////////////////////////////////

        acc1+=4; //"mxX":
        if ((idx)<10)//if ((v+1)<10)
            acc1+=1;//mx
        else
            acc1+=2;//mxx

        if ( v < (NUM_VECTOR2SERVER-1) )
            acc1+=1;// comma between vectors

        acc+= acc1;

#ifdef V2SERVER_GETDATALENGTH_DEBUG
        Serial.println("acc is");//printf("acc is %i \n", acc);
        Serial.println( acc);
#endif
    }
    acc+=2; // open{ }close

#ifdef V2SERVER_GETDATALENGTH_DEBUG
    Serial.println();
    Serial.println( acc);
#endif
    return acc;
}

uint8_t ethernet_uploadData_job(void)
{
    if (client.connect(server, 80))
    {
        client.println(F("POST /jsondecode.php HTTP/1.1"));
        client.println(F("Host: 192.168.1.47"));//obligado for HTTP 1.1.
        //client.println(F("Host: www.firwar.com"));//obligado for HTTP 1.1.
        client.println(F("Content-Type: application/json"));//client.println("Content-Type: application/x-www-form-urlencoded; charset=utf-8");
        client.println(F("Connection: close"));
        client.println(F("User-Agent: FIRWAR/1.00"));
        //
        client.print(F("Content-Length: "));
        client.println(v2server_getDataLength());
        client.println();
        //
        v2server_postData_json();
    }
}

//#define client Serial
void v2server_postData_json(void)
{
    uint8_t idx,dimx,dimy,element_size;
    void *pointer;
    uint8_t fil;
    uint8_t col;
    char buf1[50];
    
    client.print(F("{"));

    for (uint8_t v=0; v<NUM_VECTOR2SERVER; v++)
    {
        idx       = pgm_read_byte_near((const uint8_t *) &v2server[v].idx);
        dimx    = pgm_read_byte_near((const uint8_t *) &v2server[v].dim[0]);
        dimy    = pgm_read_byte_near((const uint8_t *) &v2server[v].dim[1]);
        element_size = pgm_read_byte_near((const uint8_t *) &v2server[v].element_size);
        //pointer = (void *) pgm_read_word(&(v2server[v].pointer));
        
        if (idx==idx_mainvar_FunctionMode)
        {
          mainvar_getvalue("FunctionMode").toCharArray(buf1, 50);
          pointer = (void *)buf1;
          dimx    = strlen(buf1);
        }
        else if (idx==idx_mainvar_ApiKeyW)
        {
          mainvar_getvalue("ApiKeyW").toCharArray(buf1, 50);
          pointer = (void *)buf1;
          dimx    = strlen(buf1);
        }
        else if (idx==idx_mainvar_SerialNumber)
        {
          mainvar_getvalue("SerialNumber").toCharArray(buf1, 50);
          pointer = (void *)buf1;
          dimx    = strlen(buf1);
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
        if (v==0)
            client.print(F("\"d"));
        else
            client.print(F("\"m"));

        client.print(idx);//0,1,...
        client.print(F("\":"));
        //-----------------------------------
        if ((dimy > 0) && (dimx > 0))//array bidimensional
            client.print(F("["));
        //-----------------------------------
        for (uint8_t f=0; f<fil; f++)
        {
            client.print(F("["));
            for (uint8_t c=0; c<col ; c++)
            {
                client.print(F("\""));

                //++-print data
                if ((idx==idx_vector_dimm)||(idx==idx_ltc_connected)||(idx==idx_uv_flag)||(idx==idx_ov_flag)||(idx==idx_openwire_flag)||(idx==idx_stack_union)||(idx==idx_select_2_12V)
                    ||(idx==idx_digital_input)||(idx==idx_out_leds)||(idx==idx_do_rele)
                    ||(idx==idx_fuelsensor_model)
                    ||(idx==idx_fuelsensor_outputtype_type)||(idx==idx_fuelsensor_outputtype_length_units)||(idx==idx_fuelsensor_outputtype_volume_units)
                    ||(idx==idx_fuelsensor_tank_type)
                    ||(idx==idx_fuelsensor_tank_irregular_spline_node_units_length_units)||(idx==idx_fuelsensor_tank_irregular_spline_node_units_volume_units)
                )
                {
                    client.print(*(uint8_t *)pointer);
                }
                else if ((idx==idx_stack_voltage_ifcF)||(idx==idx_stack_currentF)||(idx==idx_stack_temperatureF)||(idx==idx_vcellF)||(idx==idx_vgpioF)
                      ||(idx==idx_internal_die_tempF)||(idx==idx_internal_socF)||(idx==idx_analog_input)
                      ||(idx==idx_fuelsensor_value)
                      ||(idx==idx_fuelsensor_tank_rectangular_area_length)||(idx==idx_fuelsensor_tank_rectangular_area_width)
                      ||(idx==idx_fuelsensor_calib_tank_innertank)||(idx==idx_fuelsensor_calib_tank_zero2full)||(idx==idx_fuelsensor_calib_tank_full2upper)
                )
                {
                    client.print(*(float *)pointer, 3);
                }
                else if  (
                      (idx==idx_time_str)||(idx==idx_date_str) 
                      ||(idx==idx_mainvar_FunctionMode)
                      ||(idx==idx_mainvar_ApiKeyW)
                      ||(idx==idx_mainvar_SerialNumber)
                      
                )
                {
                    client.print(*(char *)pointer);
                }
                //--+

                client.print(F("\""));

                if (col>1)// # elements>1
                {
                    if (c< (col-1))
                        client.print(F(","));
                }
                pointer = (uint8_t *)pointer + element_size;
            }
            client.print(F("]"));

            if ((dimy > 0) && (dimx > 0))//array bidimensional
            {
                if (f<(fil-1))
                    client.print(F(","));
            }
        }

        //-----------------------------------
        if (dimy > 0)//array bidimensional
            client.print(F("]"));
        //-----------------------------------

        if (NUM_VECTOR2SERVER>0)
        {
            if (v< (NUM_VECTOR2SERVER-1))
                client.print(F(","));
        }

    }
    client.println(F("}"));

}

