//Copyright 2022(c) John Sullivan


//handles all ADC calls
//JTS2doLater: Remove Arduino functions
//JTS2doLater: use interrupts, so ADC calls don't block execution (each conversion takes ~112 us)

#include "muddersMIMA.h"

/////////////////////////////////////////////////////////////////////

uint16_t adc_readJoystick_rawCounts(void)
{
	return analogRead(PIN_USER_JOYSTICK);
} 

/////////////////////////////////////////////////////////////////////
