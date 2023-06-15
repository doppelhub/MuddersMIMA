//Copyright 2022-2023(c) John Sullivan


//config.h - compile time configuration parameters

#ifndef config_h
	#define config_h
	#include "muddersMIMA.h"  //For Arduino IDE compatibility

	#define FW_VERSION "0.0.7"
    #define BUILD_DATE "2023JUN10"

	#define CPU_MAP_ATMEGA328p
    
	//#define HW_PROTO
    #define HW_REVA_REVB

	#define LED_NORMAL
	//#define LED_DEBUG

#endif