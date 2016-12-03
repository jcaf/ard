#ifndef STACKCURRENT_H_
#define STACKCURRENT_H_

    float get_stack_current(uint16_t output_xgain);
    uint8_t current_over_permissible_limit(float current, float temperature);

#endif // STACKCURRENT_H_
