//Copyright 2022-2023(c) John Sullivan


//engine related signals

#include "muddersMIMA.h"

volatile uint16_t latestEngineRPM = 0;

/////////////////////////////////////////////////////////////////////////////////////////////

//PCMSK0 is configured so that only D8 causes interrupt (supports D8:D13)
ISR(PCINT0_vect)
{
	static uint32_t tachometerTick_previous_us = 0;

	if(gpio_engineRPM_getPinState() == HIGH)
	{
		uint32_t tachometerTick_now_us = micros();

		uint32_t periodBetweenTicks_us = tachometerTick_now_us - tachometerTick_previous_us;

		latestEngineRPM = ONE_MINUTE_IN_MICROSECONDS / periodBetweenTicks_us;

		tachometerTick_previous_us = tachometerTick_now_us;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

uint16_t engineSignals_getLatestRPM(void) { return latestEngineRPM; }

/////////////////////////////////////////////////////////////////////////////////////////////

void engineSignals_begin(void)
{
	cli();
	PCMSK0 = (1<<PCINT0); //only pin D8 will generate a pin change interrupt on ISR PCINT0_vect (which supports D8:D13)
	PCICR |= (1<<PCIE0); //enable pin change interrupts on port B (D8:D13)
	sei();
}

/////////////////////////////////////////////////////////////////////////////////////////////

void engineSignals_handler(void)
{
	;
}