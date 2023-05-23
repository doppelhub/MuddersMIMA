//Copyright 2022(c) John Sullivan


#ifndef adc_h
	#define adc_h

	#define JOYSTICK_GUARDBAND               15 //additional guardband to prevent running up to limits
	#define JOYSTICK_MAX_COUNTS              (922 + JOYSTICK_GUARDBAND) //joystick only outputs up to 90% of VCC (1024*0.9=922)
	#define JOYSTICK_MIN_COUNTS              (102 - JOYSTICK_GUARDBAND) //joystick only outputs dn to 10% of VCC (1024*0.1=102) 
	#define JOYSTICK_NEUTRAL_NOM_COUNTS      511 //midband
	#define JOYSTICK_NEUTRAL_ACCURACY_COUNTS 41 //specified: ±4% (1024*.04=41 counts)
	#define JOYSTICK_NEUTRAL_MAX_COUNTS      ((uint16_t)((JOYSTICK_NEUTRAL_NOM_COUNTS + JOYSTICK_NEUTRAL_ACCURACY_COUNTS) + JOYSTICK_GUARDBAND))
	#define JOYSTICK_NEUTRAL_MIN_COUNTS      ((uint16_t)((JOYSTICK_NEUTRAL_NOM_COUNTS - JOYSTICK_NEUTRAL_ACCURACY_COUNTS) - JOYSTICK_GUARDBAND))
	#define ADC_NUM_COUNTS_10b               1024

	uint16_t adc_readJoystick_rawCounts(void);

	uint16_t adc_readJoystick_invertedCounts(void);

#endif
