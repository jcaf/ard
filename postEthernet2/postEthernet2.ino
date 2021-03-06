#include <Ethernet.h>
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };
byte server[] = { 192,168,1,34 };
EthernetClient client;

void v2server_postData_json(void);

#define NUM_BOARD 4
#define NUM_IC NUM_BOARD*2
#define LTC_NUM_CELLS 12
#define NUM_CELL_2V_MAX LTC_NUM_CELLS//12
#define NUM_CELL_12V_MAX 4
#define NUM_TOTAL_REGAUX (5+1) //5 GPIO + 1 VREF2
#define NUM_BYTES_IN_REGGROUP 6
#define ADD_PEC 2

uint8_t ltc_connected[NUM_IC]={1,0,1,0,1,0,1,0};
float   stack_voltage_ifcF[NUM_IC]={12.123, 11.123, 10.123, 9.123, 12.123, 11.123, 10.123, 9.123};//ifc=included factor corr.
float   stack_currentF[NUM_IC];
float   stack_temperatureF[NUM_IC];
uint8_t select_2_12V[NUM_IC];
uint8_t stack_union[NUM_IC];

float   vcellF[NUM_IC][LTC_NUM_CELLS];
uint8_t uv_flag[NUM_IC][LTC_NUM_CELLS];
uint8_t ov_flag[NUM_IC][LTC_NUM_CELLS];
uint8_t openwire_flag[NUM_IC][LTC_NUM_CELLS];
float   vgpioF[NUM_IC][NUM_TOTAL_REGAUX];

void setup() 
{
  Serial.begin(230400);//9600);
  ethernet_init();
  
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
#define TIME_STR_LENGTH 12
#define DATE_STR_LENGTH 9
char time_str[TIME_STR_LENGTH]="12:31:28 AM";//hh:mm:ss am0
char date_str[DATE_STR_LENGTH]="08/06/16";//dd/mm/yy0

#define NUM_VECTOR2SERVER 13
const struct _v2server
{
    const uint8_t m;//1..
    const void *pointer;
    const uint8_t dim[2];//fil,col
    const uint8_t element_size;
}
v2server[NUM_VECTOR2SERVER] PROGMEM=
{
    {1, (uint8_t *)ltc_connected,{NUM_IC,0              }, sizeof(ltc_connected[0])  },
    {2, (float *)stack_voltage_ifcF,{NUM_IC,0              }, sizeof(stack_voltage_ifcF[0])  },

    {3, (float *)stack_currentF,{NUM_IC,0              }, sizeof(stack_currentF[0])  },
    {4, (float *)stack_temperatureF,{NUM_IC,0              }, sizeof(stack_temperatureF[0])  },
    {5, (uint8_t *)select_2_12V,{NUM_IC,0              }, sizeof(select_2_12V[0])  },
    {6, (uint8_t *)stack_union,{NUM_IC,0              }, sizeof(stack_union[0])  },

    {7, (float *)vcellF,{NUM_IC,LTC_NUM_CELLS  }, sizeof(vcellF[0][0])      },
    {8, (uint8_t *)uv_flag,{NUM_IC,LTC_NUM_CELLS  }, sizeof(uv_flag[0][0])      },
    {9, (uint8_t *)ov_flag,{NUM_IC,LTC_NUM_CELLS  }, sizeof(ov_flag[0][0])      },
    {10, (uint8_t *)openwire_flag,{NUM_IC,LTC_NUM_CELLS  }, sizeof(openwire_flag[0][0])      },
    {11, (float *)vgpioF,{NUM_IC,NUM_TOTAL_REGAUX}, sizeof(vgpioF[0][0])      },

    {12, (char *)time_str,{TIME_STR_LENGTH-1,0}, sizeof(time_str[0])},
    {13, (char *)date_str,{DATE_STR_LENGTH-1,0}, sizeof(date_str[0])},
    //{12, (float *)var,{0,0}, sizeof(var)},
};

//#define V2SERVER_GETDATALENGTH_DEBUG
uint16_t v2server_getDataLength(void)
{
    uint8_t m,dimx,dimy,element_size;
    void *pointer;
    char buf[12];
    uint16_t acc=0;
    uint16_t acc1;
    //
    uint8_t fil;
    uint8_t col;

    for (uint8_t v=0; v<NUM_VECTOR2SERVER ; v++ )
    {
        /*
        m = v2server[v].m;
        dimx = v2server[v].dim[0];
        dimy = v2server[v].dim[1];
        element_size = v2server[v].element_size;
        pointer = (void *)v2server[v].pointer;
        */

        m       = pgm_read_byte_near((const uint8_t *) &v2server[v].m);
        dimx    = pgm_read_byte_near((const uint8_t *) &v2server[v].dim[0]);
        dimy    = pgm_read_byte_near((const uint8_t *) &v2server[v].dim[1]);
        element_size = pgm_read_byte_near((const uint8_t *) &v2server[v].element_size);
        pointer = (void *) pgm_read_word(&(v2server[v].pointer));

#ifdef V2SERVER_GETDATALENGTH_DEBUG
        Serial.println(m);
        Serial.println(dimx);
        Serial.println(dimy);
        Serial.println(element_size);
#endif

        //# of bytes containing inside vectors
        if ((dimx>0) || (dimy>0))
        {
            if ((m==1)||(m==6))
            {
                acc += dimx;
            }
            else if ( (m==8)||(m==9)||(m==10) )
            {
                acc += (dimx*dimy);
            }
            else if (m==12)
            {
                acc += dimx;//time_str;
            }
            else if (m==13)
            {
                acc += dimx;//date_str
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
                        if ((m==2)||(m==3)||(m==4)||(m==7)||(m==11))
                        {
                            //acc += sprintf(buf, "%.3f", *(float *)pointer);
                            dtostrf(*(float *)pointer,4,3,buf);
                            acc+= strlen(buf);

#ifdef V2SERVER_GETDATALENGTH_DEBUG
                            Serial.print(*(float *)pointer,3);
                            Serial.print(" ");
#endif
                        }
                        else if (m==5)
                        {
                            //acc += sprintf(buf, "%u", *(uint8_t *)pointer);
                            itoa(*(uint8_t *)pointer, buf, 10);
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
            //acc += sprintf(buf, "%u", *(uint8_t *)v2server[v].pointer);
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
        if ((m)<10)//if ((v+1)<10)
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
        //client.println(F("Host: 192.168.1.34"));//obligado for HTTP 1.1.
        client.println(F("Host: www.firwar.com"));//obligado for HTTP 1.1.
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
    uint8_t m,dimx,dimy,element_size;
    void *pointer;
    uint8_t fil;
    uint8_t col;

    client.print(F("{"));

    for (uint8_t v=0; v<NUM_VECTOR2SERVER; v++)
    {
        m       = pgm_read_byte_near((const uint8_t *) &v2server[v].m);
        dimx    = pgm_read_byte_near((const uint8_t *) &v2server[v].dim[0]);
        dimy    = pgm_read_byte_near((const uint8_t *) &v2server[v].dim[1]);
        element_size = pgm_read_byte_near((const uint8_t *) &v2server[v].element_size);
        pointer = (void *) pgm_read_word(&(v2server[v].pointer));

        //-----------------------------------
        //interchange of dimension
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
        //-----------------------------------
        client.print(F("\"m"));
        client.print(v+1);
        client.print(F("\":"));
        //-----------------------------------
        if (dimy > 0)//array bidimensional
            client.print(F("["));
        //-----------------------------------
        for (uint8_t f=0; f<fil; f++)
        {
            client.print(F("["));
            for (uint8_t c=0; c<col ; c++)
            {
                client.print(F("\""));

                //++-print data
                if ((m==1)||(m==8)||(m==9)||(m==10)||(m==6)||(m==5) )
                {
                    client.print(*(uint8_t *)pointer);
                }
                else if ((m==2)||(m==3)||(m==4)||(m==7)||(m==11))
                {
                    client.print(*(float *)pointer, 3);
                }
                else if  ( (m==12)||(m==13) )
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

            if (dimx>0)//array bidimensional
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

