//Copyright 2022-2023(c) John Sullivan


//handles all ADC calls
//JTS2doLater: Remove Arduino functions
//JTS2doLater: use interrupts, so ADC calls don't block execution (each conversion takes ~112 us)

#include "muddersMIMA.h"

/////////////////////////////////////////////////////////////////////

uint16_t adc_readJoystick_rawCounts(void)
{
	uint16_t rawCounts_ADC = JOYSTICK_NEUTRAL_NOM_COUNTS;

	return analogRead(PIN_USER_JOYSTICK);
} 

/////////////////////////////////////////////////////////////////////

//OEM signal is inverted compared to how potentiometer is installed in car
uint16_t adc_readJoystick_invertedCounts(void)
{
	uint16_t rawCounts_ADC = (adc_readJoystick_rawCounts() & (ADC_NUM_COUNTS_10b - 1)); //store 10 LSBs for inversion
	uint16_t invertedCounts_ADC = (ADC_NUM_COUNTS_10b - 1) - rawCounts_ADC; //invert counts

	if( (invertedCounts_ADC + rawCounts_ADC) != (ADC_NUM_COUNTS_10b - 1) )
	{
		//something overflowed
		//this should never happen, but worth checking
		invertedCounts_ADC = JOYSTICK_NEUTRAL_NOM_COUNTS; //safe output
	}

	return invertedCounts_ADC;
} 
