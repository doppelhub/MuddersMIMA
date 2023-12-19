//Copyright 2022-2023(c) John Sullivan


//engine related signals

#include "muddersMIMA.h"

volatile uint16_t latestEngineRPM = 0;
volatile uint16_t latestVehicleMPH = 0;

/////////////////////////////////////////////////////////////////////////////////////////////

//PCMSK0 is configured so that only D8 causes interrupt (supports D8:D13)
ISR(PCINT0_vect)
{
	static uint32_t tachometerTick_previous_us = 0;

	if(gpio_engineRPM_getPinState() == HIGH)
	{
		uint32_t tachometerTick_now_us = micros();

		uint32_t periodBetweenTicks_us = tachometerTick_now_us - tachometerTick_previous_us;

		latestEngineRPM = (ONE_MINUTE_IN_MICROSECONDS * NUM_ENGINE_REVOLUTIONS_PER_CYCLE / NUM_TACHOMETER_PULSES_PER_CYCLE) / periodBetweenTicks_us;

		tachometerTick_previous_us = tachometerTick_now_us;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

ISR(PCINT2_vect)
{
	static uint32_t vssTick_previous_us = 0;

	if(gpio_VSS_getPinState() == HIGH)
	{
		uint32_t vssTick_now_us = micros();

		uint32_t periodBetweenTicks_us = vssTick_now_us - vssTick_previous_us;

		latestVehicleMPH = ONE_HOUR_IN_MICROSECONDS / periodBetweenTicks_us / 4250;

		vssTick_previous_us = vssTick_now_us;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

uint16_t engineSignals_getLatestRPM(void) { return latestEngineRPM; }

/////////////////////////////////////////////////////////////////////////////////////////////

uint16_t engineSignals_getLatestVehicleMPH(void) { return latestVehicleMPH; }

/////////////////////////////////////////////////////////////////////////////////////////////

void engineSignals_begin(void)
{
	cli();
	PCMSK0 = (1<<PCINT0); //only pin D8 will generate a pin change interrupt on ISR PCINT0_vect (which supports D8:D13)
	PCICR |= (1<<PCIE0); //enable pin change interrupts on port B (D8:D13)

	PCMSK2 = (1<<PCINT23); //On port D, only pin D7 will generate a pin change interrupt, ISR PCINT2_vect
	PCICR |= (1<<PCIE2); //enable pin change interrupts on port D (D8:D13)
	sei();
}

/////////////////////////////////////////////////////////////////////////////////////////////

void engineSignals_handler(void)
{
	;
}