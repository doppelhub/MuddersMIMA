//Copyright 2022-2023(c) John Sullivan


#ifndef engine_signals_h
	#define engine_signals_h

	#define ONE_HOUR_IN_MICROSECONDS 3600E6
	#define ONE_MINUTE_IN_MICROSECONDS 60E6
	#define NUM_ENGINE_REVOLUTIONS_PER_CYCLE 2
	#define NUM_TACHOMETER_PULSES_PER_CYCLE 3
	
	void engineSignals_begin(void);

	void engineSignals_handler(void);

	uint16_t engineSignals_getLatestRPM(void);

	uint16_t engineSignals_getLatestVehicleMPH(void);
#endif