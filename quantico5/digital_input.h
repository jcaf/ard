#ifndef DIGITAL_INPUT_H_
#define DIGITAL_INPUT_H_

	#define NUM_DIG_INPUT_MAX 4
	extern uint8_t digital_input[NUM_DIG_INPUT_MAX];
	void digital_input_job(void);
	void digital_input_init(void);
#endif

