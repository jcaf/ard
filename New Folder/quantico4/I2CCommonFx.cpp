/****************************************************************************
* Project: ${PROJECT_FILENAME}
*
*
*
* Created: 13/02/2012 07:03:12 p.m.
*  Author: jcaf
*
* Copyright 2012 Juan Carlos Agüero Flores
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
#include "I2C.h"
#include "I2CCommonFx.h"


//Buffer I2C - general purpose
//volatile byte I2Ccfx_UserBuff[I2Ccfx_MAX_ELEM_USERBUFF];

//
static void _I2Ccfx_InitSeqReading_(byte SLAVE_ADDRESS, byte START_ADDRESS);
/********************************************************************************
MEMORY ADDRESS: 1 byte
DATA: 1 byte
********************************************************************************/
void I2Ccfx_Write1Byte(byte SLAVE_ADDRESS, byte START_ADDRESS, byte DATA)
{
    Start_I2C_unimaster();
    TxAddressSlave_I2C_unimaster((SLAVE_ADDRESS & 0xFE)| WRITE_I2C_MASTER);
    TxData_I2C_unimaster(START_ADDRESS);
    TxData_I2C_unimaster(DATA);
    Stop_I2C_unimaster();
}
/********************************************************************************
MEMORY ADDRESS: 1 byte
DATA: Array [max 256 elementos]
********************************************************************************/
void I2Ccfx_WriteArray(byte SLAVE_ADDRESS, byte START_ADDRESS, volatile byte *pDATA, byte NUMBYTES_TOWRITE)
{
    Start_I2C_unimaster();
    TxAddressSlave_I2C_unimaster((SLAVE_ADDRESS & 0xFE)| WRITE_I2C_MASTER);
    TxData_I2C_unimaster(START_ADDRESS);
    for (volatile byte i=0; i<NUMBYTES_TOWRITE; i++)
    {
        TxData_I2C_unimaster(pDATA[i]);
    }
    Stop_I2C_unimaster();
}

////////////////////////////// LECTURA ////////////////////////////
static void _I2Ccfx_InitSeqReading_(byte SLAVE_ADDRESS, byte START_ADDRESS)
{
    Start_I2C_unimaster();
    TxAddressSlave_I2C_unimaster(SLAVE_ADDRESS | WRITE_I2C_MASTER);
    TxData_I2C_unimaster(START_ADDRESS);	//Direccion interna
    Restart_I2C_unimaster();
    TxAddressSlave_I2C_unimaster(SLAVE_ADDRESS | READ_I2C_MASTER);
}

void I2Ccfx_ReadRegistersAtAddress(byte SLAVE_ADDRESS, byte START_ADDRESS, volatile byte * pDATA, byte NUMBYTES_TOREAD)
{
    byte Count_NumBytesRead=0;

    _I2Ccfx_InitSeqReading_((SLAVE_ADDRESS & 0xFE), START_ADDRESS);

    do
    {
        Count_NumBytesRead++;

        //Dejar preparado el ACK/NACK para el Ste. evento
        if (Count_NumBytesRead < NUMBYTES_TOREAD)
        {
            RxData_I2C_unimaster(_ACK_);
        }
        else
        {
            RxData_I2C_unimaster(_NACK_);   //Finalizar recepción
        }

        //*pDATA = _TWDR;	//RxData_I2C_unimaster	actualiza variable global _TWDR
        *pDATA = TWDR;
        pDATA++;
    }
    while (Count_NumBytesRead < NUMBYTES_TOREAD);

    Stop_I2C_unimaster();
}

