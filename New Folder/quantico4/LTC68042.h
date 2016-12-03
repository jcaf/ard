#ifndef LTC68042_H
#define LTC68042_H
    //-------------------------
    #include "board.h"
    #define NUM_IC NUM_BOARD*2
    //-------------------------

    #define LTC_NUM_CELLS 12
    #define NUM_CELL_2V_MAX LTC_NUM_CELLS//12
    #define NUM_CELL_12V_MAX 4

    #define NUM_TOTAL_REGAUX (5+1) //5 GPIO + 1 VREF2

    #define NUM_BYTES_IN_REGGROUP 6
    #define ADD_PEC 2


    #define LTC6804_CS 5    //pin 5 ArduinoMEGA

    #define output_low(pin)   digitalWrite(pin, LOW)
    #define output_high(pin)  digitalWrite(pin, HIGH)
    #define input(pin)        digitalRead(pin)

    #define MD_FAST 1
    #define MD_NORMAL 2
    #define MD_FILTERED 3

    #define CELL_CH_ALL 0
    #define CELL_CH_1and7 1
    #define CELL_CH_2and8 2
    #define CELL_CH_3and9 3
    #define CELL_CH_4and10 4
    #define CELL_CH_5and11 5
    #define CELL_CH_6and12 6

    #define AUX_CH_ALL 0
    #define AUX_CH_GPIO1 1
    #define AUX_CH_GPIO2 2
    #define AUX_CH_GPIO3 3
    #define AUX_CH_GPIO4 4
    #define AUX_CH_GPIO5 5
    #define AUX_CH_VREF2 6

    //read voltage cell
    #define READ_CELL_GROUP_ABCD 0
    #define READ_CELL_GROUP_A 1
    #define READ_CELL_GROUP_B 2
    #define READ_CELL_GROUP_C 3
    #define READ_CELL_GROUP_D 4

    //read aux register: GPIO+VREF2
    #define READ_AUX_GROUP_AB 0
    #define READ_AUX_GROUP_A 1
    #define READ_AUX_GROUP_B 2

    //CHST[2:0] Status Group Selection
    #define STAT_CH_ALL     0
    #define STAT_CH_SOC     1
    #define STAT_CH_ITMP    2
    #define STAT_CH_VA      3
    #define STAT_CH_VD      4

    void set_adax(uint8_t MD, uint8_t CHG );
    void set_adcv(uint8_t MD, uint8_t DCP, uint8_t CH);
    void set_stat(uint8_t MD, uint8_t CHST );
    void set_adow(uint8_t MD, uint8_t PUP,uint8_t DCP,uint8_t CH);

    void    LTC6804_ADAX_broadcast();
    void    LTC6804_ADSTAT_broadcast(void);

    int8_t  LTC6804_RDAUX_BYGROUP_address(uint8_t address, uint8_t aux_group, uint16_t *data_ext);
    int8_t  LTC6804_RDCV_BYGROUP_address(uint8_t address, uint8_t rdcv_group, uint16_t *data_ext);
    void    LTC6804_ADCV_address(uint8_t address);
    void    LTC6804_ADOW_address(uint8_t address);
    int8_t  LTC6804_RDSTAT_address(uint8_t address, char group, uint8_t *data_ext);
    void    LTC6804_WRCFG_address(uint8_t address, uint8_t * config_reg_group);

    void    ltc_write_cfg_address(uint8_t address, float VUV, float VOV, uint16_t DCC12_1 );
    int8_t  ltc_rdaux_groupAB_allIC(uint16_t (*aux_codes)[NUM_TOTAL_REGAUX]);

#endif

