//Copyright 2022-2023(c) John Sullivan

//handles all ADC calls

#include "muddersMIMA.h"

////////////////////////////////////////////////////////////////////////////////////

//JTS2doNow: If behavior is jerky, change all joystick and cmdpwr functions from 'percent' to '8b_binaryPercent' 
uint8_t adc_readJoystick_percent(void)
{
	uint16_t adcResult_counts = analogRead(PIN_USER_JOYSTICK); //10b ADC
	uint8_t percent = (uint8_t)(adcResult_counts * 0.0978); //counts/1023*100

	if(percent > 100) { percent = 100;}

	#ifdef INVERT_JOYSTICK_DIRECTION
		percent = 100 - percent;
	#endif

	return percent;
}

////////////////////////////////////////////////////////////////////////////////////

uint8_t adc_getCMDPWR_percent(void)
{ 
	uint16_t adcResult_counts = analogRead(PIN_CMDPWR_ECM); //10b ADC
	uint8_t percent = (uint8_t)(adcResult_counts * 0.0978); //counts/1023*100

	if(percent > 100) { percent = 100; }
	
	return percent;
}

////////////////////////////////////////////////////////////////////////////////////

uint8_t adc_getMAMODE1_percent(void)
{
	uint16_t adcResult_counts = analogRead(PIN_MAMODE1_ECM); //10b ADC
	uint8_t percent = (uint8_t)(adcResult_counts * 0.0978); //counts/1023*100

	if(percent > 100) { percent = 100; }

	return percent;
}

////////////////////////////////////////////////////////////////////////////////////