//Copyright 2022-2023(c) John Sullivan


//config.h - compile time configuration parameters

#ifndef config_h
	#define config_h
	#include "muddersMIMA.h"  //For Arduino IDE compatibility

	#define FW_VERSION "0.1.9"
    #define BUILD_DATE "2024MAY30"

	#define CPU_MAP_ATMEGA328p
    
    #define HW_REVA_REVB

	#define INVERT_JOYSTICK_DIRECTION //comment to mirror joystick assist and regen directions

	//choose behavior when three position switch...
	//...is in the '0' position
		  #define MODE0_BEHAVIOR() mode_OEM()
		//#define MODE0_BEHAVIOR() mode_manualControl_new();
		//#define MODE0_BEHAVIOR() mode_manualControl_old();

	//...is in the '1' position
		//#define MODE1_BEHAVIOR() mode_OEM()
	  	  #define MODE1_BEHAVIOR() mode_manualControl_new();
		//#define MODE1_BEHAVIOR() mode_manualControl_old();

	//...is in the '2' position
		//#define MODE2_BEHAVIOR() mode_OEM()
		//#define MODE2_BEHAVIOR() mode_manualControl_new();
	  	  #define MODE2_BEHAVIOR() mode_manualControl_old();

#endif