//Copyright 2022-2023(c) John Sullivan

#include "muddersMIMA.h"

uint8_t brakeLightMode = BRAKE_LIGHT_AUTOMATIC;

////////////////////////////////////////////////////////////////////////////////////

void brakeLights_setControlMode(uint8_t newMode) { brakeLightMode = newMode; }

////////////////////////////////////////////////////////////////////////////////////

void pulseBrakeLights(void)
{
	static int16_t pwmOutput_8bCounts = 0;
	static bool isCountingUp = true;
	uint8_t deltaCountsPerLoop = 10;

	if(isCountingUp == true) { pwmOutput_8bCounts += deltaCountsPerLoop; }
	else                     { pwmOutput_8bCounts -= deltaCountsPerLoop; }

	if(pwmOutput_8bCounts > 255) { pwmOutput_8bCounts = 255; }
	if(pwmOutput_8bCounts <   0) { pwmOutput_8bCounts =   0; }

	if(pwmOutput_8bCounts == 255 ) { isCountingUp = false; }
	if(pwmOutput_8bCounts ==   0 ) { isCountingUp = true;  }

	analogWrite(PIN_BRAKE, pwmOutput_8bCounts);
}

////////////////////////////////////////////////////////////////////////////////////

uint8_t brakeLights_handler(void)
{
	if(brakeLightMode == BRAKE_LIGHT_AUTOMATIC)
	{
		//JTS2doNow: When brake pressed, this logic causes gpio_getBrakePosition_bool() to alternate between "Lights ON" & "Lights OFF" results
		if     (gpio_getMCM_CMDPWR_percent() < TURN_BRAKE_LIGHTS_ON_BELOW_CMDPWR_VALUE_PERCENT) { gpio_brakeLights_turnOn();   }
		else if(gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_ON)                            { gpio_brakeLights_turnOff();  }
		else                                                                                    { gpio_brakeLights_floatPin(); }
	}
	else if(brakeLightMode == BRAKE_LIGHT_FORCE_ON)  { gpio_brakeLights_turnOn();  }
	else if(brakeLightMode == BRAKE_LIGHT_FORCE_OFF) { gpio_brakeLights_turnOff(); }
	else if(brakeLightMode == BRAKE_LIGHT_PULSE)     { pulseBrakeLights();         }

}