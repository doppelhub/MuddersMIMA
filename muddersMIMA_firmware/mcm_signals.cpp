//Copyright 2022(c) John Sullivan


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

//open drain signal
void mcm_setCMDPWR_percent(uint8_t newPercent)
{
	uint16_t counts = newPercent * 10.24; //10b counter (percent*1024/100)

	if(counts > 1023) { counts = 1023; }

	gpio_setCMDPWR_counts(counts);
}

/////////////////////////////////////////////////////////////////////////////////////////////

//open drain signal
void mcm_setMAMODE2_state(uint8_t newState)
{
	digitalWrite(PIN_MAMODE2_MCM, newState);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void mcm_setMAMODE1_state(uint8_t newState)
{
	mcm_setMAMODE1_percent(newState);
}
