//Copyright 2022(c) John Sullivan


//debug stuff

#include "muddersMIMA.h"

/////////////////////////////////////////////////////////////////////////////////////////////

//open drain signal generated by ECM
uint8_t ecm_getMAMODE1_percent(void)
{
	uint16_t adcResult_counts = gpio_getMAMODE1_counts();
	uint8_t adcResult_percent = (uint8_t)(adcResult_counts * 0.098); //10b ADC (counts/1024*100)

	return adcResult_percent;
}

/////////////////////////////////////////////////////////////////////////////////////////////

//open drain signal generated by ECM
uint8_t ecm_getCMDPWR_percent(void)
{
	uint16_t adcResult_counts = gpio_getCMDPWR_counts();
	uint8_t adcResult_percent = adcResult_counts * 0.098; //10b ADC (counts/1024*100)

	return adcResult_percent;
}

/////////////////////////////////////////////////////////////////////////////////////////////

//open drain signal generated by ECM
uint8_t ecm_getMAMODE2_state(void)
{
	if( gpio_getMAMODE2() ) { return MAMODE2_STATE_IS_REGEN_STANDBY; }
	else                    { return MAMODE2_STATE_IS_ASSIST; }
}

/////////////////////////////////////////////////////////////////////////////////////////////

uint8_t ecm_getMAMODE1_state(void)
{
	uint8_t percent = ecm_getMAMODE1_percent();
	uint8_t state = MAMODE1_STATE_IS_UNDEFINED;

	if     (percent <   10) { state = MAMODE1_STATE_IS_ERROR_LO;  } // 0: 10%
	else if(percent <=  20) { state = MAMODE1_STATE_IS_PRESTART;  } //10: 20%
	else if(percent <=  30) { state = MAMODE1_STATE_IS_ASSIST;    } //20: 30%
	else if(percent <=  40) { state = MAMODE1_STATE_IS_REGEN;     } //30: 40% 
	else if(percent <=  60) { state = MAMODE1_STATE_IS_IDLE;      } //40: 60%
	else if(percent <=  75) { state = MAMODE1_STATE_IS_AUTOSTOP;  } //60: 75%
	else if(percent <=  90) { state = MAMODE1_STATE_IS_START;     } //75: 90%
	else if(percent <= 100) { state = MAMODE1_STATE_IS_ERROR_HI;  } //90:100%
	else                    { state = MAMODE1_STATE_IS_UNDEFINED; } // above 100%

	return state;
}