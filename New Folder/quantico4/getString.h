#ifndef GETSTRING_H_
#define GETSTRING_H_

    String getString_voltage(float V);
    String getString_current(float I);
    String getString_temperature(float T);
    String getString_error(String Text, uint8_t E);
    String getString_cell_voltage(float CV, uint8_t num_cell);

#endif // GETSTRING_H_
