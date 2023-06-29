//Copyright 2022-2023(c) John Sullivan


#ifndef adc_h
	#define adc_h

	#define ADC_NUM_COUNTS_10b               1023

	#define JOYSTICK_MAX_ALLOWED_PERCENT      95 //joystick only outputs up   to 90% of VCC //+2% guardband
	#define JOYSTICK_MIN_ALLOWED_PERCENT       5 //joystick only outputs down to 10% of VCC //-2% guardband
	#define JOYSTICK_NEUTRAL_NOM_PERCENT      50 //resting joystick position
	#define JOYSTICK_NEUTRAL_ACCURACY_PERCENT  5 //datasheet specifies ±4%
	#define JOYSTICK_NEUTRAL_MAX_PERCENT      (JOYSTICK_NEUTRAL_NOM_PERCENT + JOYSTICK_NEUTRAL_ACCURACY_PERCENT)
	#define JOYSTICK_NEUTRAL_MIN_PERCENT      (JOYSTICK_NEUTRAL_NOM_PERCENT - JOYSTICK_NEUTRAL_ACCURACY_PERCENT)

	uint8_t adc_readJoystick_percent(void);

	uint8_t adc_getCMDPWR_percent(void);

	uint8_t adc_getMAMODE1_percent(void);

#endif
