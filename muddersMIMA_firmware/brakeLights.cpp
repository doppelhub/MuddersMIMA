//Copyright 2022-2023(c) John Sullivan

#include "muddersMIMA.h"

////////////////////////////////////////////////////////////////////////////////////

uint8_t brakeLights_handler(void)
{
	if     (gpio_getMCM_CMDPWR_percent() < TURN_BRAKE_LIGHTS_ON_BELOW_CMDPWR_VALUE_PERCENT) { gpio_brakeLights_turnOn();   }
	else if(gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_ON)                            { gpio_brakeLights_turnOff();  }
	else                                                                                    { gpio_brakeLights_floatPin(); }
}