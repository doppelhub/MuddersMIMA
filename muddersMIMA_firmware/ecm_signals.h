//Copyright 2022-2023(c) John Sullivan


#ifndef ecm_signals_h
	#define ecm_signals_h
	
	#define MAMODE1_STATE_IS_ERROR_LO  10
	#define MAMODE1_STATE_IS_PRESTART  15
	#define MAMODE1_STATE_IS_ASSIST    25
	#define MAMODE1_STATE_IS_REGEN     35
	#define MAMODE1_STATE_IS_IDLE      50
	#define MAMODE1_STATE_IS_AUTOSTOP  70
	#define MAMODE1_STATE_IS_START     85
	#define MAMODE1_STATE_IS_ERROR_HI  90
	#define MAMODE1_STATE_IS_UNDEFINED 255

	#define MAMODE2_STATE_IS_ASSIST        0
	#define MAMODE2_STATE_IS_REGEN_STANDBY 1

	uint8_t ecm_getMAMODE1_state(void);
	bool    ecm_getMAMODE2_state(void);
	uint8_t ecm_getCMDPWR_percent(void);
	uint8_t ecm_getRemappedCMDPWR_percent(void);

	void ecm_handler(void);

#endif