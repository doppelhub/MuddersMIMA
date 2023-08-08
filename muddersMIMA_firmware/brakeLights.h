#ifndef brakeLights_h
	#define brakeLights_h

	#define TURN_BRAKE_LIGHTS_ON_BELOW_CMDPWR_VALUE_PERCENT 35 //valid range is 10:45%
	#define BRAKE_LIGHT_AUTOMATIC 1
	#define BRAKE_LIGHT_FORCE_ON  2
	#define BRAKE_LIGHT_FORCE_OFF 4
	#define BRAKE_LIGHT_PULSE     8

	void brakeLights_setControlMode(uint8_t newMode);

	uint8_t brakeLights_handler(void);

#endif
