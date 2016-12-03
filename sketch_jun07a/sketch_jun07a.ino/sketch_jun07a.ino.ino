#define TIME_STR_LENGTH 12
#define DATE_STR_LENGTH 9

char time_str[12];//hh:mm:ss am0
char date_str[9];//dd/mm/yy0
//struct _rtcc rtcc;

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

#define NUM_VECTOR2SERVER 11
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

    //{12, (char *)time_str,{TIME_STR_LENGTH-1,0}, sizeof(time_str[0])},
    //{13, (char *)date_str,{DATE_STR_LENGTH-1,0}, sizeof(date_str[0])},
    //{12, (float *)var,{0,0}, sizeof(var)},
};
#define V2SERVER_GETDATALENGTH_DEBUG
void v2server_getDataLength(void)
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
        Serial.println("---------------------");
        m = pgm_read_byte_near((const uint8_t *) &v2server[v].m);
        dimx = pgm_read_byte_near((const uint8_t *) &v2server[v].dim[0]);
        dimy = pgm_read_byte_near((const uint8_t *) &v2server[v].dim[1]);
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
            if ((m==1)||(m==8)||(m==9)||(m==10)||(m==6))
            {
                acc += dimx;
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
                            Serial.print(*(float *)pointer,3);Serial.print(" ");
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
        if (v<10)
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
    Serial.print( acc);
    #endif
}

void setup() 
{
  // put your setup code here, to run once:
Serial.begin(9600);
v2server_getDataLength();

}

void loop() {
  // put your main code here, to run repeatedly:

}
