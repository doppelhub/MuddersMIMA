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

void unlatchSignal_BRAKE_uC(void)
{
	//LiControl's high-side driver remains latched on after user removes foot from brake pedal
	//thus, the brake lights will stay on unless we briefly force the high-side driver off 
	if (gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_ON)
	{
		//either the brake pedal is pressed or the high-side driver is latched (we don't know)
		gpio_brakeLights_turnOff(); //if brake released, pulling BRAKE_uC low turns high-side driver off
		delayMicroseconds(100); //if brake released, BRAKE_RAW discharges to ground in 75 us
		//if brake pedal released, BRAKE_uC is now low (~0.7 volts due to diode drop)
		//if brake pedal pressed,  BRAKE_uC takes 200 us to pullup to 4V ~= Vih(min)
	}
	
	gpio_brakeLights_floatPin(); //allows LiControl to check brake status
}

////////////////////////////////////////////////////////////////////////////////////

uint8_t brakeLights_handler(void)
{
	if(brakeLightMode == BRAKE_LIGHT_AUTOMATIC)
	{
		uint8_t joystickPercent = adc_readJoystick_percent();

		//brake light control logic
		//JTS2doNow: When brake pressed, gpio_getBrakePosition_bool() alternates between "Lights ON" & "Lights OFF"
		if     (joystickPercent < JOYSTICK_MIN_ALLOWED_PERCENT)                { gpio_brakeLights_turnOff(); } //joystick input too low	
		else if(joystickPercent < TURN_BRAKE_LIGHTS_ON_BELOW_JOYSTICK_PERCENT) { gpio_brakeLights_turnOn();  } //strong regen //JTS2doNow: Add hysteresis
		else                                                                   { unlatchSignal_BRAKE_uC();   }
	}
	else if(brakeLightMode == BRAKE_LIGHT_MONITOR_ONLY) { unlatchSignal_BRAKE_uC();   }
	else if(brakeLightMode == BRAKE_LIGHT_OEM)          { gpio_brakeLights_turnOff(); }
	else if(brakeLightMode == BRAKE_LIGHT_FORCE_ON)     { gpio_brakeLights_turnOn();  }
	else if(brakeLightMode == BRAKE_LIGHT_PULSE)        { pulseBrakeLights();         }
}