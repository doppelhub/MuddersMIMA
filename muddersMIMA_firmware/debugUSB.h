//Copyright 2022(c) John Sullivan


#ifndef debugUSB_h
	#define debugUSB_h
	
	#define TWO_DECIMAL_PLACES 2
	#define FOUR_DECIMAL_PLACES 4

	#define DEBUG_USB_UPDATE_PERIOD_mS 100

	void debugUSB_displayUptime_seconds(void);
	void debugUSB_printButtonStates(void);
	
#endif