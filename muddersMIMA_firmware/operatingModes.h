//Copyright 2022-2023(c) John Sullivan


#ifndef modes_h
	#define modes_h
	
	#define  Assist 96 	// light assist is reduced to compensate for increase in power levels due to voltage spoofing.
	#define  Boost 	16  	// circa 10kW assist under gentle acceleration (full assist with larger throttle openings) 
	#define  Cruise 36 	// provides assist during cruise. (circa 6kW highway speeds)
	#define  Coast 	110  	// reduction in lift-off regen to compensates for increase in power levels due to voltage spoofing.
	#define  Brake 	30   	// increases brake regen to mask 3rd gear glitch & provide full regen under braking in all gears
	#define	 tpsoffset 9 	// throttle position offset at zero
	#define	 maxmph 112	// maximum MPH safeguard
				 // recommended that background regen (part throttle & forced) are be disabled in LIBCM config (REDUCE_BACKGROUND_REGEN_UNLESS_BRAKING)
	void operatingModes_handler(void);

#endif
