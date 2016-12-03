#ifndef STACKERROR_H_
#define STACKERROR_H_

    uint8_t get_stackError_UV(uint8_t num_ic);
    uint8_t get_stackError_OV(uint8_t num_ic);
    uint8_t get_stackError_openWire(uint8_t num_ic);
    uint8_t get_stackError_overCurrent(uint8_t num_ic);
    uint8_t get_stackError_overTemperature(uint8_t num_ic);

    uint8_t get_stackError_total(uint8_t num_ic);
#endif // STACKERROR_H_
