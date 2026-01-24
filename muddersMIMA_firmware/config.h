//Copyright 2022-2023(c) John Sullivan


//config.h - compile time configuration parameters

#ifndef config_h
	#define config_h
	#include "muddersMIMA.h"  //For Arduino IDE compatibility

	#define FW_VERSION "0.2.0"
    #define BUILD_DATE "2024JUN05"

	#define CPU_MAP_ATMEGA328p
    
    #define HW_REVA_REVB

	#define INVERT_JOYSTICK_DIRECTION //comment to mirror joystick assist and regen directions

	//basically the "deadzones" of the throttle position sensor
	#define MODE_ELECTRICTHROTTLE_TPSRANGE_START 12 //value from adc_getECM_TPS_percent that is treated as 0% throttle
	#define MODE_ELECTRICTHROTTLE_TPSRANGE_END 85 //value from adc_getECM_TPS_percent that is treated as 100% throttle

	//choose behavior when three position switch...
	//...is in the '0' position
		//#define MODE0_BEHAVIOR() mode_OEM()
		//#define MODE0_BEHAVIOR() mode_manualAssistRegen_withAutoStartStop();
		#define MODE0_BEHAVIOR() mode_manualAssistRegen_ignoreECM();
		//#define MODE0_BEHAVIOR() mode_INWORK_PHEV_mudder();
		//#define MODE2_BEHAVIOR() mode_electricThrottle();

	//...is in the '1' position
		//#define MODE1_BEHAVIOR() mode_OEM()
	  	  #define MODE1_BEHAVIOR() mode_manualAssistRegen_withAutoStartStop();
		//#define MODE1_BEHAVIOR() mode_manualAssistRegen_ignoreECM();
		//#define MODE1_BEHAVIOR() mode_INWORK_PHEV_mudder();
		//#define MODE2_BEHAVIOR() mode_electricThrottle();

	//...is in the '2' position
		//#define MODE2_BEHAVIOR() mode_OEM()
		//#define MODE2_BEHAVIOR() mode_manualAssistRegen_withAutoStartStop();
		//#define MODE2_BEHAVIOR() mode_manualAssistRegen_ignoreECM();
		//#define MODE2_BEHAVIOR() mode_INWORK_PHEV_mudder();
		  #define MODE2_BEHAVIOR() mode_electricThrottle();

#endif