//Copyright 2022(c) John Sullivan


//Handles serial debug data transfers from LIBCM to  host
//FYI:    Serial       data transfers from host  to LiBCM are handled elsewhere.

//JTS2doLater: Gather all Serial Monitor transmissions here

#include "muddersMIMA.h"

/////////////////////////////////////////////////////////////////////////////////////////////

void debugUSB_displayUptime_seconds(void)
{
	Serial.print(F("\nUptime(s): "));
	Serial.print( String(millis() * 0.001) );
}

/////////////////////////////////////////////////////////////////////////////////////////////

void debugUSB_printButtonStates(void)
{	
	static uint32_t previousMillis = 0;

	if( millis() - previousMillis >= DEBUG_USB_UPDATE_PERIOD_mS)
	{
		previousMillis = millis();

		Serial.print(F("\nButton States\nJoystick: "));
		Serial.print(adc_readJoystick(),DEC);
		Serial.print(F("\nMomentary: "));
		if(gpio_getButton_momentary() == BUTTON_NOT_PRESSED) { Serial.print(F("Not ")); }
		Serial.print(F("Pressed\nToggle Position: "));

		switch(gpio_getButton_toggle() )
		{
			case TOGGLE_POSITION0: Serial.print(F("0 (OEM)")); break;
			case TOGGLE_POSITION1: Serial.print(F("1 (Manual)")); break;
			case TOGGLE_POSITION2: Serial.print(F("2 (NO REGEN)")); break;
			case TOGGLE_SHORTED  : Serial.print(F("TOGGLE SWITCH SHORTED!")); break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////