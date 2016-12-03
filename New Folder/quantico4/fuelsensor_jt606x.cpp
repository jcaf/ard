/****************************************************************************
* Project: ${PROJECT_FILENAME}
*
* fuelsensor_jt606x.cpp
* Shenzhen joint technology co,ltd
* Jointech JT606 Protocol v1.4
*
*
* Created: 05/09/2016 06:03:12 p.m.
*  Author: jcaf
*
* Copyright 2016 Juan Carlos Agüero Flores
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
****************************************************************************/
#include <Arduino.h>
#include "system.h"
#include "utils.h"

#include "fuelsensor_jt606x.h"
#include "spline.h"
#include "rs485.h"


//------------------------------------------------------------------------------------------------------
    union _jt606_send_zeroAdj
    {
        struct _field
        {
            char head;//'@'
            char sensor_id[2];//ascci 00->99
            char cmd;//A-Z
            char content_length[2];//ascii-hex
            //
            char set_read;//ascii: Zero Setting for JT606. 0 means Read ,1 means Set
            //
            char checksum[2];//ascii
            char end;//'#'
        } field;
        char frame[];
    };
    #define JT606_SEND_ZERO_ADJ_FRAMELENGTH sizeof(union _jt606_send_zeroAdj)

    union _jt606_reply_zeroAdj
    {
        struct _field
        {
            char head;
            char sensor_id[2];
            char cmd;
            char content_length[2];
            //
            char internal_ad[4];//Internal AD value when emtpy fuel tank(Decimal System). it’s different from fuel level value.
            //Ignore this value when you analysis.
            //
            char checksum[2];
            char end;
        } field;
        char frame[];
    };
    #define JT606_REPLY_ZERO_ADJ_FRAMELENGTH sizeof(union _jt606_reply_zeroAdj)//13

    //------------------------------------------------------------------------------------------------------
    union _jt606_send_fullrangeAdj
    {
        struct _field
        {
            char head;
            char sensor_id[2];
            char cmd;
            char content_length[2];
            //
            char set_read;//Full-range Setting for JT606. 0 means Read ,1 means Set
            //
            char checksum[2];
            char end;
        } field;
        char frame[];
    };
    #define JT606_SEND_FULLRANGE_ADJ_FRAMELENGTH sizeof(union _jt606_send_zeroAdj)

    union _jt606_reply_fullrangeAdj
    {
        struct _field
        {
            char head;
            char sensor_id[2];
            char cmd;
            char content_length[2];
            //
            char internal_ad[4];//Internal AD value when emtpy fuel tank(Decimal System). it’s different from fuel level value.
            //Ignore this value when you analysis.
            //
            char checksum[2];
            char end;
        } field;
        char frame[];
    };
    #define JT606_REPLY_FULLRANGE_ADJ_FRAMELENGTH sizeof(union _jt606_reply_fullrangeAdj)//13

    //------------------------------------------------------------------------------------------------------
    union _jt606_send_readFuelLevel
    {
        struct _field
        {
            char head;
            char sensor_id[2];
            char cmd;
            char content_length[2];
            //
            char checksum[2];
            char end;
        } field;
        char frame[];
    };
    #define JT606_SEND_READ_FUELLEVEL_FRAMELENGTH sizeof(union _jt606_send_readFuelLevel)

    union _jt606_reply_readFuelLevel
    {
        struct _field
        {
            char head;
            char sensor_id[2];
            char cmd;
            char content_length[2];
            //
            char percentage[5];
            char advalue[4];//0-4095
            char litres[6];
            char fixed_val_zero;//0
            char reserved[3];
            //
            char checksum[2];
            char end;
        } field;

        //char frame[JT606_REPLY_LEVEL_VALUE_FRAMELENGTH];
        char frame[];
    };
    #define JT606_REPLY_READ_FUELLEVEL_FRAMELENGTH sizeof(union _jt606_reply_readFuelLevel)
    //------------------------------------------------------------------------------------------------------

struct _jt606 jt606;

const struct _jt606_cmdcode
{
    char cmd;
    uint8_t sendFrameLength;
    uint8_t replyFrameLength;
}
jt606_cmdcode[JT606_CMD_CODE_NUM_MAX] PROGMEM=
{
    {JT606_CMD_CODE_SET_BAUDRATE,0,0},
    {JT606_CMD_CODE_ZERO_ADJ,JT606_SEND_ZERO_ADJ_FRAMELENGTH, JT606_REPLY_ZERO_ADJ_FRAMELENGTH},
    {JT606_CMD_CODE_FULLRANGE_ADJ,JT606_SEND_FULLRANGE_ADJ_FRAMELENGTH,JT606_REPLY_FULLRANGE_ADJ_FRAMELENGTH},
    {JT606_CMD_CODE_SET_FUELTANKVOL,0,0},
    {JT606_CMD_CODE_READ_FUELLEVEL,JT606_SEND_READ_FUELLEVEL_FRAMELENGTH,JT606_REPLY_READ_FUELLEVEL_FRAMELENGTH},
    {JT606_CMD_CODE_READSET_SENSORID,0,0},
    {JT606_CMD_CODE_READSET_DAMPING,0,0},
    {JT606_CMD_CODE_FACTORY_RESET,0,0},
};

static uint8_t jt606_cmdcode_get_sendFrameLength(char cmd)
{
    for (uint8_t i=0; i<JT606_CMD_CODE_NUM_MAX; i++)
    {
        if (cmd == pgm_read_byte_near(&jt606_cmdcode[i].cmd))
            return pgm_read_byte_near(&jt606_cmdcode[i].sendFrameLength);
    }
    return 0;
}
static uint8_t jt606_cmdcode_get_replyFrameLength(char cmd)
{
    for (uint8_t i=0; i<JT606_CMD_CODE_NUM_MAX; i++)
    {
        if (cmd == pgm_read_byte_near(&jt606_cmdcode[i].cmd))
            return pgm_read_byte_near(&jt606_cmdcode[i].replyFrameLength);
    }
    return 0;
}

static uint8_t jt606_calc_checksum(char *data, uint8_t num_byte)
{
    uint8_t i;
    int16_t checksum=0x0000;

    for (i=0; i<num_byte; i++)
        checksum += data[i];

    return (uint8_t)checksum;
}

//#define JT606_CMD_SEND_DEBUG_PRINT
static inline void jt606_cmd_send(char *frame, uint8_t frame_length)
{
    #ifdef JT606_CMD_SEND_DEBUG_PRINT
    sysPrint_serialPrint("sending cmd: ");
    #endif // JT606_CMD_SEND_DEBUG_PRINT

    for (uint8_t i=0; i< frame_length; i++)
    {
        Serial2.write(frame[i]);
        delayMicroseconds(1000);//>=800

        #ifdef JT606_CMD_SEND_DEBUG_PRINT
        sysPrint_serialPrint(String(frame[i]));
        #endif // JT606_CMD_SEND_DEBUG_PRINT
    }
    #ifdef JT606_CMD_SEND_DEBUG_PRINT
    sysPrint_serialPrintln("");
    sysPrint_serialPrintln("sending cmd: finish");
    #endif // JT606_CMD_SEND_DEBUG_PRINT
}

//#define JT606_GET_REPLY_DEBUG_PRINT
static uint8_t jt606_get_incomming_frame(char *frame, uint8_t frame_length)
{
    char c=0;
    uint8_t cod_ret = 0;

    static uint8_t sm0 = 0;
    static size_t i;

    if (Serial2.available())
    {
        c = Serial2.read();

        #ifdef JT606_GET_REPLY_DEBUG_PRINT
        sysPrint_serialPrintln(String(c));
        #endif // JT606_GET_REPLY_DEBUG_PRINT

        if (sm0 == 0)
        {
            if (c == '@')
            {
                frame[0] = c;
                i=1;
                sm0++;
            }
        }
        else if (sm0 == 1)
        {
            frame[i] = c;

            if ( i< (frame_length-1) )
            {
                if (frame[i] == '#')
                {
                    sm0 = 0x00;
                    #ifdef JT606_GET_REPLY_DEBUG_PRINT
                    sysPrint_serialPrintln("jt606_get_incomming_frame ERROR");
                    #endif // JT606_GET_REPLY_DEBUG_PRINT
                }
            }
            else
            {
                if (frame[i] == '#')
                {
                    cod_ret = 1;
                    sm0 = 0x00;
                }
            }

            i++;
        }
    }

    #ifdef JT606_GET_REPLY_DEBUG_PRINT
    if (cod_ret == 1)
    {
        sysPrint_serialPrint("reply frame is: ");
        for (uint8_t i=0; i<frame_length; i++)
            sysPrint_serialPrint(String(frame[i]));

        sysPrint_serialPrintln("");
    }
    #endif // JT606_GET_REPLY_DEBUG_PRINT

    return cod_ret;
}

static uint8_t jt606_cmd_reply(char *frame, uint8_t frame_length)
{
    if (jt606_get_incomming_frame(frame, frame_length))
    {
        uint8_t checksum = jt606_calc_checksum(&frame[1], frame_length-4);

        if ( bin_to_asciihex(checksum>>4) == frame[frame_length-3])
        {
            if ( bin_to_asciihex(checksum&0x0F) == frame[frame_length-2])
                return 1;
        }

    }
    return 0;
}
//#define SEND_AND_REPLY_DEBUG_PRINT
static uint8_t send_and_reply(char *sendframe, char *replyframe)//sequential
{
    //static
    uint8_t sm0 = 0;
    //static
    unsigned long last_millis;
    uint8_t cod_ret=  0;

    char cmd = sendframe[3];
    uint8_t sendframe_length = jt606_cmdcode_get_sendFrameLength(cmd);
    uint8_t replyframe_length = jt606_cmdcode_get_replyFrameLength(cmd);

    #ifdef SEND_AND_REPLY_DEBUG_PRINT
    sysPrint_serialPrintln("cmd code is:" + String(cmd));
    sysPrint_serialPrintln("sendframe_length is:" + String(sendframe_length));
    sysPrint_serialPrintln("replyframe_length is:" + String(replyframe_length));
    #endif // SEND_AND_REPLY_DEBUG_PRINT

    if (sm0 == 0)
    {
        digitalWrite(RS485_DIR, RS485_TX);

        jt606_cmd_send(sendframe, sendframe_length);

        delayMicroseconds(800);
        digitalWrite(RS485_DIR, RS485_RX);

        last_millis = millis();
        sm0++;
    }

    if (sm0 == 1)
    {
        #ifdef SEND_AND_REPLY_DEBUG_PRINT
        sysPrint_serialPrintln("Serial received buffer is: ");
        #endif // SEND_AND_REPLY_DEBUG_PRINT

        while ( (millis() - last_millis) < 30000)//30s
        {
            if ( jt606_cmd_reply(replyframe, replyframe_length) )
            {
                cod_ret = 1;
                digitalWrite(RS485_DIR, RS485_TX);
                break;
            }
        }
    }
    return cod_ret;
}


/********************************************//**
 * @brief Send command: zero adjustment
 *
 * @param set_read char: Set=ASCII '1', Read=ASCII '0'
 * @return uint8_t 1:Success, else 0
 *
 ***********************************************/
//#define JT606_SEND_ZEROADJ_DEBUG_PRINT
uint8_t jt606_send_zeroAdj(char set_read)
{
    uint8_t cod_ret=0;
    #define JT606_CMD_SET_ZERO_ADJ      '@','0','1','B','0','1','1','X','X','#'//@01B01135# -> reply=@01B04XXXXxx#
    #define JT606_CMD_READ_ZERO_ADJ     '@','0','1','B','0','1','0','X','X','#'//@01B01034#

    union _jt606_send_zeroAdj jt606_send_zeroAdj =    {JT606_CMD_SET_ZERO_ADJ};
    union _jt606_reply_zeroAdj jt606_reply_zeroAdj;
    //
    char cmd = jt606_send_zeroAdj.field.cmd;//JT606_CMD_CODE_ZERO_ADJ
    jt606_send_zeroAdj.field.set_read = set_read;//ascii
    uint8_t checksum = jt606_calc_checksum(&jt606_send_zeroAdj.frame[1], jt606_cmdcode_get_sendFrameLength(cmd)-4);
    jt606_send_zeroAdj.field.checksum[0]=  bin_to_asciihex(checksum>>4);
    jt606_send_zeroAdj.field.checksum[1]=  bin_to_asciihex(checksum&0x0F);
    //

    #ifdef JT606_SEND_ZEROADJ_DEBUG_PRINT
    sysPrint_serialPrintln("[CFLS JT606X]");
    String msg="send cmd: ZERO_ADJ";
    (set_read == '1')? msg += String("(SET)") : msg += String("(READ)");
    sysPrint_serialPrint(msg);
    #endif // JT606_SEND_ZEROADJ_DEBUG_PRINT

    if (send_and_reply(jt606_send_zeroAdj.frame, jt606_reply_zeroAdj.frame))
        if ((jt606_reply_zeroAdj.field.cmd == cmd) && (jt606_reply_zeroAdj.field.content_length[1] == '4'))
            cod_ret =  1;

    #ifdef JT606_SEND_ZEROADJ_DEBUG_PRINT
    sysPrint_serialPrint(msg);
    (cod_ret==1)? sysPrint_serialPrintln("OK"):sysPrint_serialPrintln("ERROR");
    #endif // JT606_SEND_ZEROADJ_DEBUG_PRINT

    return cod_ret;
}

/********************************************//**
 * @brief Send command: fullrange adjustment
 *
 * @param set_read char: Set=ASCII '1', Read=ASCII '0'
 * @return uint8_t 1:Success, else 0
 *
 * Take 22s
 ***********************************************/
//#define JT606_SEND_FULLRANGEADJ_DEBUG_PRINT
uint8_t jt606_send_fullrangeAdj(char set_read)
{
    uint8_t cod_ret=0;
    #define JT606_CMD_SET_FULLRANGE_ADJ      '@','0','1','C','0','1','1','X','X','#'
    #define JT606_CMD_READ_FULLRANGE_ADJ     '@','0','1','C','0','1','0','X','X','#'

    union _jt606_send_fullrangeAdj jt606_send_fullrangeAdj =    {JT606_CMD_SET_FULLRANGE_ADJ};
    union _jt606_reply_fullrangeAdj jt606_reply_fullrangeAdj;
    //
    char cmd = jt606_send_fullrangeAdj.field.cmd;//JT606_CMD_CODE_FULLRANGE_ADJ
    jt606_send_fullrangeAdj.field.set_read = set_read;//ascii
    uint8_t checksum = jt606_calc_checksum(&jt606_send_fullrangeAdj.frame[1], jt606_cmdcode_get_sendFrameLength(cmd)-4);
    jt606_send_fullrangeAdj.field.checksum[0]=  bin_to_asciihex(checksum>>4);
    jt606_send_fullrangeAdj.field.checksum[1]=  bin_to_asciihex(checksum&0x0F);
    //

    #ifdef JT606_SEND_FULLRANGEADJ_DEBUG_PRINT
    sysPrint_serialPrintln("[CFLS JT606X]");
    String msg="send cmd: FULLRANGE_ADJ";
    (set_read == '1')? msg += String("(SET)") : msg += String("(READ)");
    sysPrint_serialPrint(msg);
    #endif // JT606_SEND_FULLRANGEADJ_DEBUG_PRINT

    if (send_and_reply(jt606_send_fullrangeAdj.frame, jt606_reply_fullrangeAdj.frame))
        if ((jt606_reply_fullrangeAdj.field.cmd == cmd) && (jt606_reply_fullrangeAdj.field.content_length[1] == '4'))
            cod_ret =  1;

    #ifdef JT606_SEND_FULLRANGEADJ_DEBUG_PRINT
    sysPrint_serialPrint(msg);
    (cod_ret==1)? sysPrint_serialPrintln("OK"):sysPrint_serialPrintln("ERROR");
    #endif // JT606_SEND_FULLRANGEADJ_DEBUG_PRINT

    return cod_ret;
}


/********************************************//**
 * @brief Send command: jt606_send_readFuelLevel
 *
 * @return uint8_t 1:Success, else 0
 *
 ***********************************************/
//#define JT606_SEND_READFUELLEVEL_DEBUG_PRINT
uint8_t jt606_send_readFuelLevel(void)
{
    uint8_t cod_ret=0;
    #define JT606_CMD_READ_READFUELLEVEL_ADJ      '@','0','1','E','0','0','0','6','#'

    union _jt606_send_readFuelLevel jt606_send_readFuelLevel =    {JT606_CMD_READ_READFUELLEVEL_ADJ};
    union _jt606_reply_readFuelLevel jt606_reply_readFuelLevel;
    //
    char cmd = jt606_send_readFuelLevel.field.cmd;
    //
    #ifdef JT606_SEND_READFUELLEVEL_DEBUG_PRINT
    sysPrint_serialPrintln("[CFLS JT606X]");
    String msg="send cmd: READ FUEL LEVEL";
    sysPrint_serialPrintln(msg);
    #endif // JT606_SEND_READFUELLEVEL_DEBUG_PRINT

    if (send_and_reply(jt606_send_readFuelLevel.frame, jt606_reply_readFuelLevel.frame))
    {
        if ((jt606_reply_readFuelLevel.field.cmd == cmd) )//&& (jt606_reply_readFuelLevel.field.content_length[0] == '1') && (jt606_reply_readFuelLevel.field.content_length[1] == '3'))
        {
            cod_ret =  1;
            //
            char percentage_ASCII[7];
            percentage_ASCII[0] = jt606_reply_readFuelLevel.field.percentage[0];
            percentage_ASCII[1] = jt606_reply_readFuelLevel.field.percentage[1];
            percentage_ASCII[2] = jt606_reply_readFuelLevel.field.percentage[2];
            percentage_ASCII[3] = '.';
            percentage_ASCII[4] = jt606_reply_readFuelLevel.field.percentage[3];
            percentage_ASCII[5] = jt606_reply_readFuelLevel.field.percentage[4];
            percentage_ASCII[6] = '\0';

            jt606.internalSensor.percentage = strtod(percentage_ASCII, (char**)0);

            #ifdef JT606_SEND_READFUELLEVEL_DEBUG_PRINT
            sysPrint_serialPrintln("fuel level percentage: " +String(jt606.internalSensor.percentage));
            #endif // JT606_SEND_READFUELLEVEL_DEBUG_PRINT
            sysPrint_serialPrintln("Fuel level %: " +String(jt606.internalSensor.percentage));
            //
        }
    }

    #ifdef JT606_SEND_READFUELLEVEL_DEBUG_PRINT
    sysPrint_serialPrint(msg);
    (cod_ret==1)? sysPrint_serialPrintln(" OK"):sysPrint_serialPrintln(" ERROR");
    #endif // JT606_SEND_READFUELLEVEL_DEBUG_PRINT

    return cod_ret;
}


void jt606_run(void)
{
    Serial2.begin(9600);
    //Serial.begin(9600);
    Serial.begin(230400);

    digitalWrite(RS485_DIR, RS485_RX);
    pinMode(RS485_DIR, OUTPUT);


system.funct_mode = FUNCT_MODE_DEBUG;

    //_getlevel_();
    //jt606_send_zeroAdj(JT606_CMD_SET);
    //jt606_send_fullrangeAdj(JT606_CMD_SET);
    //jt606_send_readFuelLevel();
    while (1)
    {
        if (jt606_send_readFuelLevel())
            Serial.println("Y");
        else
            Serial.println("N");

        delay(500);
    }
}


void jt606_init(void)
{
    Serial2.begin(9600);//RS485 baud

    digitalWrite(RS485_DIR, RS485_RX);
    pinMode(RS485_DIR, OUTPUT);

    //system.funct_mode = FUNCT_MODE_DEBUG;
}

//void jt606_get_levelvalue(void)//if reply is automatic
//{
//    union _jt606_reply_readFuelLevel jt606_reply_readFuelLevel;
//
//    char percentage_ASCII[7];
//    float percentage;
//
//    if (jt606_cmd_reply(jt606_reply_readFuelLevel.frame, jt606_cmdcode_get_replyFrameLength(JT606_CMD_CODE_READ_FUELLEVEL)))
//    {
//        percentage_ASCII[0] = jt606_reply_readFuelLevel.field.percentage[0];
//        percentage_ASCII[1] = jt606_reply_readFuelLevel.field.percentage[1];
//        percentage_ASCII[2] = jt606_reply_readFuelLevel.field.percentage[2];
//        percentage_ASCII[3] = '.';
//        percentage_ASCII[4] = jt606_reply_readFuelLevel.field.percentage[3];
//        percentage_ASCII[5] = jt606_reply_readFuelLevel.field.percentage[4];
//        percentage_ASCII[6] = '\0';
//
//        percentage =strtod(percentage_ASCII, (char**)0);
//
//        //Serial.println(String(percentage));
//        sysPrint_serialPrintln(String(percentage));
//    }
//}
//void _getlevel_(void)
//{
//    while (1)
//    {
//        delay(2000);
//
//        digitalWrite(RS485_DIR, RS485_TX);
//
//        jt606_get_levelvalue();
//
//        digitalWrite(RS485_DIR, RS485_RX);
//    }
//}
