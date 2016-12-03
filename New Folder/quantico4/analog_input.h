#ifndef ANALOG_INPUT_H_
#define ANALOG_INPUT_H_
	
	#define NUM_ANALOG_INPUT_MAX 4
	extern uint8_t analog_input[NUM_ANALOG_INPUT_MAX];
	void analog_input_job(void);

#endif