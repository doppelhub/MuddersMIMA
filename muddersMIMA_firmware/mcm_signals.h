//Copyright 2022(c) John Sullivan


#ifndef mcm_signals_h
	#define mcm_signals_h
	
	//all pin #defines are in ecm_signals.h

	#define MCM_MAMODE2_STATE_IS_ASSIST        0
	#define MCM_MAMODE2_STATE_IS_REGEN_STANDBY 1

	void mcm_setMAMODE1_percent(uint8_t newPercent);
	void mcm_setMAMODE1_state(uint8_t newState);
	void mcm_setMAMODE2_state(uint8_t newState);
	void mcm_setCMDPWR_percent(uint8_t newPercent);

	void mcm_setAllSignals(uint8_t newState, uint16_t CMDPWR_counts);

#endif