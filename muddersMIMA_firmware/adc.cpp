//Copyright 2022-2023(c) John Sullivan

//handles all ADC calls

#include "muddersMIMA.h"

//////////////////////////////////////////////////////////////////////////////////// 

//JTS2doLater: If more resolution required, change all instances from 'percent' to 'permille' (‰)
uint8_t adc_read10bValue_Percent(int adcChannel)
{
	uint16_t adcResult_counts = analogRead(adcChannel); //10b ADC
	uint8_t percent = (uint8_t)(adcResult_counts * 0.0978); //(counts/1023)*100

	if(percent > 100) { percent = 100; }

	return percent;
}

//////////////////////////////////////////////////////////////////////////////////// 

uint8_t adc_readJoystick_percent(void)
{
	uint8_t joystick_percent = adc_read10bValue_Percent(PIN_USER_JOYSTICK);

	#ifdef INVERT_JOYSTICK_DIRECTION
		joystick_percent = 100 - joystick_percent;
	#endif

	return joystick_percent;
}

////////////////////////////////////////////////////////////////////////////////////

uint8_t adc_getECM_CMDPWR_percent (void) { return adc_read10bValue_Percent(PIN_CMDPWR_ECM)  + ADC_HARDWARE_CORRECTION_CMDPWR_PERCENT;  }
uint8_t adc_getECM_MAMODE1_percent(void) { return adc_read10bValue_Percent(PIN_MAMODE1_ECM) + ADC_HARDWARE_CORRECTION_MAMODE1_PERCENT; }
uint8_t adc_getECM_MAP_percent    (void) { return adc_read10bValue_Percent(PIN_MAP_SENSOR ); }
uint8_t adc_getECM_TPS_percent    (void) { return adc_read10bValue_Percent(PIN_THROTTLE   ); }