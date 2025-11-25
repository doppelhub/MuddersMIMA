//Copyright 2022-2023(c) John Sullivan


#ifndef modes_h
	#define modes_h
	
	#define	 tpsoffset 9 	// throttle position offset at zero
	#define	 maxmph 112	// maximum MPH safeguard
	#define  regenfactor 11	// regen power level (inverse)
	#define  assistfactor 5	// assist power level (inverse)
	#define  minrpm 250  // reduces effect of regen at low engine speed.

	void operatingModes_handler(void);

#endif
