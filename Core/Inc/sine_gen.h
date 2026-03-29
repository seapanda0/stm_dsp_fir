#ifndef INC_SINE_GEN_H_
#define INC_SINE_GEN_H_

#include "main.h"
#include <math.h>

#define ENABLE_NOISE        1   // 1 = enable, 0 = disable
#define ENABLE_WHITE_NOISE  0   // 1 = enable, 0 = disable

#define TIM_CLK 84000000

#define E_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062

#define SIGNAL_TABLE_SIZE		256							//size of sinewave table (MUST BE 2^N)
#define SIGNAL_AMPLITUDE 	  20480							//10-bit scale
#define SIGNAL_OFFSET 		  32768							//signal offset (10-bit ADC scale)
#define SIGNAL_FREQ				 10							//Hz (sine frequency)

#define NOISE_TABLE_SIZE	256 				    //size of noise table (MUST BE 2^N)
#define NOISE_AMPLITUDE 	5120							//8-bit scale
#define NOISE_OFFSET   		5120							//noise offset
#define NOISE_FREQ		    100							//Hz (noise frequency)
#define NOISE_TARGET      (NOISE_FREQ / SIGNAL_FREQ)

#define WHITE_NOISE_AMPLITUDE  25600
#define WHITE_NOISE_DURATION   10
#define WHITE_NOISE_TARGET     150

extern uint16_t SIGNAL_TABLE[SIGNAL_TABLE_SIZE];

#if ENABLE_NOISE == 1
extern uint16_t NOISE_TABLE[NOISE_TABLE_SIZE];
#endif

void sine_gen_init(TIM_HandleTypeDef *_calc_tim, TIM_HandleTypeDef *_pwm_tim);

#endif /* INC_SINE_GEN_H_ */
