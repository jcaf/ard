#ifndef FUELSENSOR_JT606X_H_
#define FUELSENSOR_JT606X_H_

#define JT606_CMD_SET '1'  //ASCII
#define JT606_CMD_READ '0' //ASCII

#define JT606_CMD_CODE_SET_BAUDRATE      'A'
#define JT606_CMD_CODE_ZERO_ADJ          'B'
#define JT606_CMD_CODE_FULLRANGE_ADJ     'C'
#define JT606_CMD_CODE_SET_FUELTANKVOL   'D'
#define JT606_CMD_CODE_READ_FUELLEVEL    'E'
#define JT606_CMD_CODE_READSET_SENSORID  'G'
#define JT606_CMD_CODE_READSET_DAMPING   'H'
#define JT606_CMD_CODE_FACTORY_RESET     'T'
//
#define JT606_CMD_CODE_NUM_MAX 8

//------------------------------------------------------------------------------------------------------
struct _jt606
{
    struct _internalSensor
    {
        float percentage;
        //float volume;
    } internalSensor;

};
extern struct _jt606 jt606;


void jt606_run(void);
void jt606_init(void);

uint8_t jt606_send_readFuelLevel(void);
uint8_t jt606_send_fullrangeAdj(char set_read);
uint8_t jt606_send_zeroAdj(char set_read);

#endif // FUELSENSOR_JT606X_H_
