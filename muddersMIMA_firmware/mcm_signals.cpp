//Copyright 2022-2023(c) John Sullivan


//debug stuff

#include "muddersMIMA.h"

/////////////////////////////////////////////////////////////////////////////////////////////

//open drain signal
void mcm_setMAMODE1_percent(uint8_t newPercent)
{
	uint16_t counts = (uint16_t)newPercent * 2.56; //8b counter (percent*256/100)

	if(counts > 255) { counts = 255; }

	gpio_setMAMODE1_counts( (uint8_t)counts );
}

/////////////////////////////////////////////////////////////////////////////////////////////

void mcm_setMAMODE1_state(uint8_t newState)
{
	mcm_setMAMODE1_percent(newState);
}

/////////////////////////////////////////////////////////////////////////////////////////////

//open drain signal
void mcm_setCMDPWR_percent(uint8_t newPercent)
{
	uint16_t counts = newPercent * 10.24; //10b counter (percent*1024/100)

	if(counts > 1023) { counts = 1023; }

	gpio_setCMDPWR_counts(counts);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void mcm_setCMDPWR_counts(uint16_t newCounts)
{
	gpio_setCMDPWR_counts(newCounts);
}

/////////////////////////////////////////////////////////////////////////////////////////////

//open drain signal
void mcm_setMAMODE2_state(uint8_t newState)
{
	digitalWrite(PIN_MAMODE2_MCM, newState);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void mcm_setAllSignals(uint8_t newState, uint16_t CMDPWR_counts)
{
	mcm_setMAMODE1_state(newState); //always the same

	if     (newState == MAMODE1_STATE_IS_ASSIST) { mcm_setMAMODE2_state(MAMODE2_STATE_IS_ASSIST);        mcm_setCMDPWR_counts(CMDPWR_counts); }
	else if(newState == MAMODE1_STATE_IS_REGEN)  { mcm_setMAMODE2_state(MAMODE2_STATE_IS_REGEN_STANDBY); mcm_setCMDPWR_counts(CMDPWR_counts); }
	else if(newState == MAMODE1_STATE_IS_IDLE)   { mcm_setMAMODE2_state(MAMODE2_STATE_IS_REGEN_STANDBY); mcm_setCMDPWR_percent(50);           }
	else
	{
		//handle all other states
	}

}