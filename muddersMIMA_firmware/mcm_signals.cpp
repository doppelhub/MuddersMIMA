//Copyright 2022-2023(c) John Sullivan


//debug stuff

#include "muddersMIMA.h"

/////////////////////////////////////////////////////////////////////////////////////////////

void mcm_setMAMODE1_state (uint8_t newState  ) { gpio_setMCM_MAMODE1_percent(newState);   } //JTS2doNow: Redundant... remove
void mcm_setMAMODE2_state (uint8_t newState  ) { gpio_setMCM_MAMODE2_bool   (newState);   }
void mcm_setCMDPWR_percent(uint8_t newPercent) { gpio_setMCM_CMDPWR_percent (newPercent); }

/////////////////////////////////////////////////////////////////////////////////////////////

void mcm_setAllSignals(uint8_t newState, uint16_t CMDPWR_percent)
{
	mcm_setMAMODE1_state(newState);

	if(CMDPWR_percent > 90) { CMDPWR_percent = 90; }
	if(CMDPWR_percent < 10) { CMDPWR_percent = 10; }

	if     (newState == MAMODE1_STATE_IS_ASSIST) { mcm_setMAMODE2_state(MAMODE2_STATE_IS_ASSIST);        mcm_setCMDPWR_percent(CMDPWR_percent); }
	else if(newState == MAMODE1_STATE_IS_REGEN)  { mcm_setMAMODE2_state(MAMODE2_STATE_IS_REGEN_STANDBY); mcm_setCMDPWR_percent(CMDPWR_percent); }
	else if(newState == MAMODE1_STATE_IS_IDLE)   { mcm_setMAMODE2_state(MAMODE2_STATE_IS_REGEN_STANDBY); mcm_setCMDPWR_percent(50);             }
	else
	{
		; //add additional states if needed
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////

void mcm_passUnmodifiedSignals_fromECM(void)
{
	mcm_setMAMODE1_state  (ecm_getMAMODE1_state()  );
	mcm_setMAMODE2_state  (ecm_getMAMODE2_state()  );
	mcm_setCMDPWR_percent (ecm_getCMDPWR_percent() );
}