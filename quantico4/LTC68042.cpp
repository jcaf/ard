#include <Arduino.h>
#include "LTC68042.h"
#include <avr/pgmspace.h>
//#define _LTC68042_DEBUG_

uint8_t ADCV[2];
uint8_t ADAX[2];
uint8_t ADSTAT[2];
uint8_t ADOW[2];

static void wakeup_idle();
static void wakeup_sleep();
static uint16_t pec15_calc(uint8_t len, uint8_t *data);
static void spi_write_array( uint8_t length, uint8_t *data);
static void spi_write_read(uint8_t *TxData, uint8_t TXlen, uint8_t *rx_data, uint8_t RXlen);
static void spi_write(uint8_t  data);
static uint8_t spi_read(uint8_t  data);

static int8_t LTC6804_rdcvGroup_address(uint8_t address, uint8_t rdcv_group, uint16_t *data_ext);
static void LTC6804_rdauxGroup_address(uint8_t address, uint8_t aux_group, uint8_t *data);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void set_adcv(uint8_t MD, uint8_t DCP, uint8_t CH)
{
    ADCV[0] = 0x02 | ((MD & 0x02)>>1);
    ADCV[1] = ((MD&0x01)<<7) | 0x60 | (DCP<<4) | (CH&0x07);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//CMD0 CMD1 PEC0 PEC1
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void LTC6804_ADCV_address(uint8_t address)
{
    uint8_t cmd[4];
    uint16_t temp_pec;

    cmd[0] = 0x80 | (address<<3) + ADCV[0];
    cmd[1] = ADCV[1];

    temp_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(temp_pec >> 8);//PEC0 (HIGH)
    cmd[3] = (uint8_t)(temp_pec);//PEC1 (LOW)

    wakeup_idle ();

    output_low(LTC6804_CS);
    spi_write_array(4,cmd);
    output_high(LTC6804_CS);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void set_adax(uint8_t MD, uint8_t CHG )//broadcast
{
    ADAX[0] = 0x04 | ((MD&0x02)>>1);
    ADAX[1] = ((MD&0x01)<<7) | 0x60 | (CHG) ;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void LTC6804_ADAX_broadcast()
{
    uint8_t cmd[4];
    uint16_t temp_pec;

    cmd[0] = ADAX[0];
    cmd[1] = ADAX[1];
    temp_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(temp_pec >> 8);
    cmd[3] = (uint8_t)(temp_pec);

    wakeup_idle ();

    output_low(LTC6804_CS);
    spi_write_array(4,cmd);
    output_high(LTC6804_CS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
int8_t LTC6804_RDCV_BYGROUP_address(uint8_t address, uint8_t rdcv_group, uint16_t *data_ext)
{
    uint8_t cv;
    int8_t pec_error;

    uint16_t data[NUM_CELL_2V_MAX];

    pec_error=0;
    cv=0;

    if (rdcv_group == READ_CELL_GROUP_ABCD )
    {
        for (uint8_t group=1; group<= 4; group++)
        {
            if (LTC6804_rdcvGroup_address(address, group, data) == -1)
            {
                pec_error = -1;
            }

            for (uint8_t i=0; i<3; i++)
            {
                data_ext[cv] = data[i];
                cv++;
            }
        }
    }
    else
    {
        pec_error = LTC6804_rdcvGroup_address(address, rdcv_group, data_ext);
    }
    return pec_error;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
int8_t LTC6804_rdcvGroup_address(uint8_t address, uint8_t rdcv_group, uint16_t *data_ext)
{
    uint8_t cmd[4];
    uint8_t data[NUM_BYTES_IN_REGGROUP + ADD_PEC];
    uint16_t temp_pec;
    uint16_t received_pec, data_pec;
    int8_t pec_error;
    uint16_t CXV;
    uint8_t c;

    if (rdcv_group == 1)
        cmd[1] = 0x04;
    else if(rdcv_group == 2)
        cmd[1] = 0x06;
    else if(rdcv_group == 3)
        cmd[1] = 0x08;
    else if(rdcv_group == 4)
        cmd[1] = 0x0A;


    wakeup_idle ();

    cmd[0] = 0x80 | (address<<3); //Setting address
    temp_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(temp_pec >> 8);
    cmd[3] = (uint8_t)(temp_pec);

    output_low(LTC6804_CS);
    spi_write_read(cmd,4, data, NUM_BYTES_IN_REGGROUP + ADD_PEC);
    output_high(LTC6804_CS);

    c=0;
    for (uint8_t i=0; i<3; i++)
    {
        CXV = (data[c+1]<<8) | data[c];
        data_ext[i] = CXV ;
        c+=2;
    }

    //b.iii
    received_pec = ( ((uint16_t)data[6])<<8 ) | data[7];//PEC0(HIGH) | PEC1(LOW)
    data_pec = pec15_calc(NUM_BYTES_IN_REGGROUP, &data[0]);


    pec_error = 0;
    if  (received_pec != data_pec)
    {
        pec_error = -1;

#ifdef _LTC68042_DEBUG_
        Serial.println(F("ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR PEC"));
#endif // _LTC68042_DEBUG_
    }

    return pec_error;
}

int8_t  ltc_rdaux_groupAB_allIC(uint16_t (*aux_codes)[NUM_TOTAL_REGAUX])
{
    int8_t pec_error = 0;

    for (uint8_t num_ic=0; num_ic<NUM_IC; num_ic++)
    {
        if ( LTC6804_RDAUX_BYGROUP_address( num_ic, READ_AUX_GROUP_AB, &aux_codes[num_ic][0] ) == -1)
            pec_error = -1;
    }
    return pec_error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//0: Read back all auxiliary registers
//1: Read back auxiliary group A
//2: Read back auxiliary group B
// IC1 GPIO1- IC1 GPIO2 -IC1 GPIO3 -IC1 GPIO4 -IC1 GPIO5 -IC1 Vref2 -> IC2 GPIO1 - IC2 GPIO2  .....
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#define GPIO_IN_REG  3
int8_t LTC6804_RDAUX_BYGROUP_address(uint8_t address, uint8_t aux_group, uint16_t *data_ext)
{
    uint8_t data[NUM_BYTES_IN_REGGROUP+ADD_PEC];//6 + 2(PEC)

    uint16_t data_pec, received_pec;

    uint8_t gpio,i;
    int8_t pec_error = 0;

    if (aux_group == READ_AUX_GROUP_AB )
    {
        gpio=0;

        for (uint8_t group=READ_AUX_GROUP_A; group<=READ_AUX_GROUP_B; group++)
        {
            LTC6804_rdauxGroup_address(address, group, data);//data recoge 8 bytes
            i=0;
            for (uint8_t gp=0; gp<GPIO_IN_REG; gp++)
            {
                data_ext[gpio++] = ( data[i+1]<<8) | data[i];
                i+=2;
            }

            received_pec =  ( data[6]<<8) | data[7];
            data_pec = pec15_calc(6, data);

            if (data_pec != received_pec)
                pec_error = -1;
        }
    }
    else
    {
        LTC6804_rdauxGroup_address(address, aux_group, data);
        i=0;
        for (uint8_t gpio=0; gpio<GPIO_IN_REG; gpio++)
        {
            data_ext[gpio] = (data[i+1]<<8) | data[i];
            i=i+2;
        }

        received_pec = ( data[i]<<8) | data[i+1];
        data_pec = pec15_calc(6, data);

        if (data_pec != received_pec)
            pec_error = -1;
    }
#ifdef _LTC68042_DEBUG_
    if (pec_error == -1)
        Serial.println("pec_error == -1");
#endif // _LTC68042_DEBUG_

    return (pec_error);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//1: Read back auxiliary group A
//2: Read back auxiliary group B
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void LTC6804_rdauxGroup_address(uint8_t address, uint8_t aux_group, uint8_t *data)
{
    uint8_t cmd[4];
    uint16_t cmd_pec;

    if (aux_group == 1)
        cmd[1] = 0x0C;
    else if(aux_group == 2)
        cmd[1] = 0x0E;

    wakeup_idle ();

    cmd[0] = 0x80 | (address<<3);
    cmd_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);

    output_low(LTC6804_CS);
    spi_write_read(cmd,4,data,8);
    output_high(LTC6804_CS);
}


/////////////////////////////////////////////////////////////////////////////////////
//configurable
//no uso set_config() -> todo en 1
/////////////////////////////////////////////////////////////////////////////////////

#define WRCFG_GPIO5_1 0x1F //GPIO5-GPIO4-GPIO3-GPIO2-GPIO1 PULL DOWN (1=DISABLE)
#define WRCFG_REFON 0//0
#define WRCFG_ADCOPT 0//0
//#define WRCFG_DCC12_1 0//x0FFF //12 discharge-cells actived
#define WRCFG_DCTO 0  //disabled by hardware

void ltc_write_cfg_address(uint8_t address, float VUV, float VOV, uint16_t DCC12_1 )
{
    uint8_t cfg[NUM_BYTES_IN_REGGROUP + ADD_PEC];
    uint16_t temp_pec;

    uint16_t vuv= (uint16_t)  ( ( (VUV/100E-6)/16) -1);
    uint16_t vov= (uint16_t)  ( (VOV/100E-6)/16);

    cfg[0]= ((WRCFG_GPIO5_1 & 0x1F)<<3) | (WRCFG_REFON<<2) |  (WRCFG_ADCOPT);
    cfg[1]=(uint8_t)vuv;
    cfg[2]=(((uint8_t)vov)<<4) |  (((uint8_t) (vuv>>8)) & 0x0F);
    cfg[3]=(uint8_t)(vov>>4);
    cfg[4]= (uint8_t)DCC12_1;
    cfg[5]=  ((WRCFG_DCTO<<4)&0xF0) | ( (DCC12_1>>8) & 0x0F);
    //ADD PEC
    temp_pec = (uint16_t)pec15_calc(NUM_BYTES_IN_REGGROUP, cfg);
    cfg[6] = (uint8_t)(temp_pec >> 8);
    cfg[7] = (uint8_t)temp_pec;

    LTC6804_WRCFG_address(address, cfg);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// The function will calculate the needed PEC codes for the write data
// and then transmit data to the ICs on a stack.
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void LTC6804_WRCFG_address(uint8_t address, uint8_t * config_reg_group) //+2 PEC
{
    uint8_t cmd[4];
    uint16_t temp_pec;

    cmd[0] = 0x80 + (address<<3); //Setting address
    cmd[1] = 0x01;
    temp_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(temp_pec >> 8);
    cmd[3] = (uint8_t)(temp_pec);

    wakeup_idle ();

    output_low(LTC6804_CS);
    spi_write_array(4,cmd);
    spi_write_array(NUM_BYTES_IN_REGGROUP + ADD_PEC, config_reg_group);
    //spi_write_array(8, config_reg_group);
    output_high(LTC6804_CS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void set_stat(uint8_t MD, uint8_t CHST)
{
    ADSTAT[0]= 0x04 | ((MD & 0x02)>>1);
    ADSTAT[1]= ((MD&0x01)<<7) | 0x68 | (CHST & 0x07);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void LTC6804_ADSTAT_broadcast(void)
{
    uint8_t cmd[4];
    uint16_t temp_pec;

    cmd[0] = ADSTAT[0];
    cmd[1] = ADSTAT[1];

    temp_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(temp_pec >> 8);
    cmd[3] = (uint8_t)(temp_pec);

    wakeup_idle ();

    output_low(LTC6804_CS);
    spi_write_array(4,cmd);
    output_high(LTC6804_CS);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
int8_t LTC6804_RDSTAT_address(uint8_t address, char group, uint8_t *data_ext)
{
    uint8_t cmd[4];
    uint8_t data[NUM_BYTES_IN_REGGROUP+ ADD_PEC];

    int8_t pec_error = 0;
    uint16_t data_pec;
    uint16_t received_pec;

    //1
    cmd[0] = 0x00;

    if (group=='A')
        cmd[1] = 0x10;//RDSTATA
    else if (group=='B')
        cmd[1] = 0x12;//RDSTATB

    //2
    wakeup_idle ();
    //3
    cmd[0] = 0x80 + (address<<3);
    data_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(data_pec >> 8);
    cmd[3] = (uint8_t)(data_pec);

    output_low(LTC6804_CS);
    spi_write_read(cmd, 4, data, NUM_BYTES_IN_REGGROUP+ ADD_PEC);
    output_high(LTC6804_CS);

    for (uint8_t current_byte=0; current_byte < NUM_BYTES_IN_REGGROUP; current_byte++)
    {
        data_ext[current_byte] = data[current_byte];
    }

    //4.b
    received_pec = (data[6]<<8) + data[7];
    data_pec = pec15_calc(NUM_BYTES_IN_REGGROUP, data_ext);

    if(received_pec != data_pec)
    {
        pec_error = -1;
    }

    return(pec_error);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void set_adow(uint8_t MD, uint8_t PUP, uint8_t DCP, uint8_t CH)
{
    ADOW[0]= 0x02 | ((MD & 0x02)>>1);
    ADOW[1]= ((MD&0x01)<<7) | ((PUP&0x01)<<6) | (1<<5) | ( (DCP&0x1)<<4) | (1<<3) | (CH & 0x07);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void LTC6804_ADOW_address(uint8_t address)
{
    uint8_t cmd[4];
    uint16_t temp_pec;

    //1
    cmd[0] = (0x80 + (address<<3)) | ADOW[0];
    cmd[1] = ADOW[1];

    //2
    temp_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(temp_pec >> 8);
    cmd[3] = (uint8_t)(temp_pec);

    //3
    wakeup_idle ();

    //4
    output_low(LTC6804_CS);
    spi_write_array(4,cmd);
    output_high(LTC6804_CS);

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generic wakeup commannd to wake isoSPI up out of idle
///////////////////////////////////////////////////////////////////////////////////////////////////////////
static void wakeup_idle()
{
    output_low(LTC6804_CS);
    delayMicroseconds(10); //Guarantees the isoSPI will be in ready mode
    output_high(LTC6804_CS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generic wakeup commannd to wake the LTC6804 from sleep
///////////////////////////////////////////////////////////////////////////////////////////////////////////
static void wakeup_sleep()
{
    output_low(LTC6804_CS);
    delay(1); // Guarantees the LTC6804 will be in standby
    output_high(LTC6804_CS);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//calculates  and returns the CRC15
//uint8_t len: the length of the data array being passed to the function
//uint8_t data* : the array of data that the PEC will be generated from
//return  The calculated pec15 as an unsigned int16_t
///////////////////////////////////////////////////////////////////////////////////////////////////////////

const uint16_t crc15Table[256] PROGMEM = {0x0,0xc599, 0xceab, 0xb32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,
                                      0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
                                      0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
                                      0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
                                      0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
                                      0x2544, 0x2be, 0xc727, 0xcc15, 0x98c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
                                      0x3d6e, 0xf8f7,0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2, 0xc25b, 0xc969, 0xcf0, 0xdf0d,
                                      0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
                                      0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
                                      0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
                                      0x4a88, 0x8f11, 0x57c, 0xc0e5, 0xcbd7, 0xe4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
                                      0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
                                      0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
                                      0x85e9, 0xf84, 0xca1d, 0xc12f, 0x4b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528,
                                      0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,
                                      0x2ac0, 0xd3a, 0xc8a3, 0xc391, 0x608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01,
                                      0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9,
                                      0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,
                                      0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,
                                      0x2fbc, 0x846, 0xcddf, 0xc6ed, 0x374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0xaf8, 0xcf61, 0xc453,
                                      0x1ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,
                                      0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,
                                      0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
                                     };
static uint16_t pec15_calc(uint8_t len, uint8_t *data)
{
    uint16_t remainder,addr;

    remainder = 16;//initialize the PEC
    for(uint8_t i = 0; i<len; i++)   // loops for each byte in data array
    {
        addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address
        remainder = (remainder<<8)^ pgm_read_word_near(&crc15Table[addr]);

    }
    //return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
    return(remainder<<1);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
static void spi_write_array(uint8_t len, // Option: Number of bytes to be written on the SPI port
     uint8_t data[] //Array of bytes to be written on the SPI port
                    )
{
    for(uint8_t i = 0; i < len; i++)
    {
        spi_write((char)data[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
static void spi_write_read(uint8_t tx_Data[],//array of data to be written on SPI port
                    uint8_t tx_len, //length of the tx data arry
                    uint8_t *rx_data,//Input: array that will store the data read by the SPI port
                    uint8_t rx_len //Option: number of bytes to be read from the SPI port
                   )
{
    for(uint8_t i = 0; i < tx_len; i++)
    {
        spi_write(tx_Data[i]);

    }

    for(uint8_t i = 0; i < rx_len; i++)
    {
        rx_data[i] = (uint8_t)spi_read(0xFF);
    }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
static void spi_write(uint8_t  data)
{
    SPDR = data;
    while (!(SPSR & _BV(SPIF)));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t spi_read(uint8_t  data)
{
    SPDR = data;
    while (!(SPSR & _BV(SPIF)));
    return SPDR;
}
