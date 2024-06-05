#ifndef brakeLights_h
	#define brakeLights_h

	#define TURN_BRAKE_LIGHTS_ON_BELOW_JOYSTICK_PERCENT 35 //valid range is 10:45%
	
	#define BRAKE_LIGHT_AUTOMATIC    1 //LiControl turns brake lights on during heavy regen
	#define BRAKE_LIGHT_FORCE_ON     2 //LiControl turns brake lights on continuously
	#define BRAKE_LIGHT_MONITOR_ONLY 3 //LiControl can    determine brake status
	#define BRAKE_LIGHT_OEM          4 //LiControl cannot determine brake status
	#define BRAKE_LIGHT_PULSE        8 //LiControl continuously pulses brake lights

	void brakeLights_setControlMode(uint8_t newMode);

	uint8_t brakeLights_handler(void);

#endif
