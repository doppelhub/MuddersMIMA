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

  	//#define SLIDER_IS_INSTALLED //Adjusts output for sliders. 20-84% range rather than 5-95%. Added for Balto.

	//Power Limiting Configuration
	#define MAX_ASSIST_POWER_WATTS_LOW_RPM 13000  // 13kW limit below RPM threshold
	#define RPM_THRESHOLD_FOR_POWER_LIMIT  2100   // RPM threshold for power limiting
	#define RPM_POWER_LIMIT_RAMP_WINDOW    200    // RPM window for ramping (2100-2300)
	#define RPM_POWER_LIMIT_HYSTERESIS     50     // RPM hysteresis to prevent oscillation
	// Note: CMDPWR_TO_WATTS_SCALING_FACTOR removed - now using actual LiBCM power data

	//High RPM Safety Configuration
	#define RPM_ASSIST_RAMPDOWN_START      5200   // RPM where assist starts ramping down
	#define RPM_ASSIST_CUTOFF              5500   // RPM where assist is completely cut off
	#define ENABLE_HIGH_RPM_ASSIST_LIMIT          // Comment out to disable high RPM limiting

	//Clutch Lockout Configuration
	#define CLUTCH_LOCKOUT_DELAY_MS        200   // Time after clutch release before allowing assist/regen
	#define CLUTCH_LOCKOUT_RAMP_TIME_MS    200    // Ramp-up time after lockout expires
	#define CLUTCH_LOCKOUT_HYSTERESIS_MS   100    // Hysteresis to prevent rapid on/off
	#define ENABLE_CLUTCH_LOCKOUT                 // Comment out to disable clutch lockout

	//choose behavior when three position switch...
	//...is in the '0' position
		  #define MODE0_BEHAVIOR() mode_OEM()
		//#define MODE0_BEHAVIOR() mode_manualAssistRegen_withAutoStartStop();
		//#define MODE0_BEHAVIOR() mode_manualAssistRegen_ignoreECM();
		//#define MODE0_BEHAVIOR() mode_blendedECM_withManualOverride();

	//...is in the '1' position
		//#define MODE1_BEHAVIOR() mode_OEM()
	  	  #define MODE1_BEHAVIOR() mode_manualAssistRegen_withAutoStartStop();
		//#define MODE1_BEHAVIOR() mode_manualAssistRegen_ignoreECM();
		//#define MODE1_BEHAVIOR() mode_blendedECM_withManualOverride();

	//...is in the '2' position
		//#define MODE2_BEHAVIOR() mode_OEM()
		//#define MODE2_BEHAVIOR() mode_manualAssistRegen_withAutoStartStop();
		//#define MODE2_BEHAVIOR() mode_manualAssistRegen_ignoreECM();
	  	  #define MODE2_BEHAVIOR() mode_blendedECM_withManualOverride();

#endif