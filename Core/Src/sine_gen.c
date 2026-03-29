#include "sine_gen.h"

uint16_t SIGNAL_TABLE[SIGNAL_TABLE_SIZE];

#if ENABLE_NOISE == 1
uint16_t NOISE_TABLE[NOISE_TABLE_SIZE];
#endif

// Save the handler pointer as global variables
TIM_HandleTypeDef *calc_tim; // Timer 3 - Update value ISR
TIM_HandleTypeDef *pwm_tim;  // Timer 4 - PWM Generation

// Compute tables for original waveform + noise and start timers
void sine_gen_init(TIM_HandleTypeDef *_calc_tim, TIM_HandleTypeDef *_pwm_tim) {
	calc_tim = _calc_tim;
	pwm_tim = _pwm_tim;
	uint16_t i;
	double angle;
	double sine_value;
	uint16_t timer_count;

#if ENABLE_NOISE == 1
	timer_count = (TIM_CLK / (NOISE_TABLE_SIZE * NOISE_FREQ)) - 1;
#else
	timer_count = (TIM_CLK / (SIGNAL_TABLE_SIZE * SIGNAL_FREQ)) - 1;
#endif

	for (i = 0; i < SIGNAL_TABLE_SIZE; i++) {
		angle = (2.0 * E_PI * i) / SIGNAL_TABLE_SIZE;
    	sine_value = sin(angle);
		SIGNAL_TABLE[i] = (uint16_t)(SIGNAL_OFFSET + SIGNAL_AMPLITUDE * sine_value);
		}

#if ENABLE_NOISE == 1
	for (i = 0; i < NOISE_TABLE_SIZE; i++) {
		angle = (2.0 * E_PI * i) / NOISE_TABLE_SIZE;
    	sine_value = sin(angle);
		NOISE_TABLE[i] = (uint16_t)(NOISE_OFFSET + NOISE_AMPLITUDE * sine_value);
		}
#endif

	calc_tim->Instance->ARR = timer_count;

	HAL_TIM_Base_Start_IT(calc_tim);
	HAL_TIM_PWM_Start(pwm_tim, TIM_CHANNEL_1);

}

void update_pwm_value() {		//compare match A ISR
	static uint8_t signal_index = 0;
#if ENABLE_NOISE == 1
	static uint8_t noise_count = 0;
	static uint8_t noise_index = 0;
#endif
#if ENABLE_WHITE_NOISE == 1
	static uint8_t white_noise_count = 0;
	static uint8_t white_noise_duration_count = 0;
	static int32_t white_noise_value = 0;
#endif

	pwm_tim->Instance->CCR1 = SIGNAL_TABLE[signal_index];

	// Signal is incremented every 10 ISR call while noise increment each time
#if ENABLE_NOISE == 1
  // Adding noise
	if(noise_count >= NOISE_TARGET)  {
    	noise_count = 0;
#endif
    	signal_index = (signal_index + 1) & (SIGNAL_TABLE_SIZE - 1);
#if ENABLE_NOISE == 1
    	}

	pwm_tim->Instance->CCR1 += NOISE_TABLE[noise_index];
	noise_index = (noise_index + 1) & (NOISE_TABLE_SIZE - 1);
	noise_count++;
#endif

#if ENABLE_WHITE_NOISE == 1
  // Adding White Noise
	if (white_noise_count >= WHITE_NOISE_TARGET) {
		white_noise_count = 0;
		white_noise_duration_count = WHITE_NOISE_DURATION;
		white_noise_value = (int32_t)(WHITE_NOISE_AMPLITUDE * ((double)((double)rand() - (double)(RAND_MAX / 2)) / (double)RAND_MAX));
		}

	if (white_noise_duration_count > 1) {
		if (white_noise_value > (65535 - pwm_tim->Instance->CCR1)) pwm_tim->Instance->CCR1 = 65535;
		else if(-white_noise_value > pwm_tim->Instance->CCR1) pwm_tim->Instance->CCR1 = 0;
		else pwm_tim->Instance->CCR1 += white_noise_value;

		white_noise_duration_count--;
		}
	white_noise_count++;
#endif
}

// When the timer responsible for calculation hits a update
void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef * htim){
	if(htim->Instance == TIM3){
		update_pwm_value();
	}
}

