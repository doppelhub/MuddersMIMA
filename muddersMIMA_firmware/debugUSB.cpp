//Copyright 2022-2023(c) John Sullivan


//Handles serial debug data transfers from LIBCM to  host
//FYI:    Serial       data transfers from host  to LiBCM are handled elsewhere.

//JTS2doLater: Gather all Serial Monitor transmissions here

#include "muddersMIMA.h"

uint8_t dataTypeToStream = DEBUGUSB_STREAM_BUTTON;
uint32_t dataUpdatePeriod_ms = 250;

/////////////////////////////////////////////////////////////////////////////////////////////

void    debugUSB_dataTypeToStream_set(uint8_t dataType) { dataTypeToStream = dataType; }
uint8_t debugUSB_dataTypeToStream_get(void) { return dataTypeToStream; }

/////////////////////////////////////////////////////////////////////////////////////////////

void     debugUSB_dataUpdatePeriod_ms_set(uint16_t newPeriod) { dataUpdatePeriod_ms = newPeriod; }
uint16_t debugUSB_dataUpdatePeriod_ms_get(void) { return dataUpdatePeriod_ms; }

/////////////////////////////////////////////////////////////////////////////////////////////

void debugUSB_displayUptime_seconds(void)
{
	Serial.print(F("\nUptime(s): "));
	Serial.print( String(millis() * 0.001) );
}

/////////////////////////////////////////////////////////////////////////////////////////////

void debugUSB_printButtonStates(void)
{	
	Serial.print(F("\nButton: "));
	if(gpio_getButton_momentary() == BUTTON_NOT_PRESSED) { Serial.print(F("UP")); }
	else                                                 { Serial.print(F("DN")); }

	Serial.print(F(", Mode: "));
	switch(gpio_getButton_toggle() )
	{
		case TOGGLE_POSITION0: Serial.print(F("0: Stock (do not modify OEM signals),   ")); break;
		case TOGGLE_POSITION1: Serial.print(F("1: Manual (ignore all OEM signals),     ")); break;
		case TOGGLE_POSITION2: Serial.print(F("2: Blended (ignore OEM assist & regen), ")); break;
		case TOGGLE_UNDEFINED: Serial.print(F("TOGGLE SWITCH SHORTED! ")); break;
	}

	Serial.print(F("Joystick: "));
	Serial.print(adc_readJoystick_rawCounts(),DEC);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void debugUSB_printOEMsignals(void)
{
	Serial.print(F("\nMAMODE2:"));
	if(ecm_getMAMODE2_state() == MAMODE2_STATE_IS_ASSIST) { Serial.print(F("Assist,  ")); }
	else                                                  { Serial.print(F("Reg/Idle,")); }

	Serial.print(F(" MAMODE1:"));
	switch( ecm_getMAMODE1_state() )
	{
		case MAMODE1_STATE_IS_ERROR_LO:  Serial.print(F("Line LO, ")); break;
		case MAMODE1_STATE_IS_PRESTART:  Serial.print(F("Prestart,")); break;
		case MAMODE1_STATE_IS_ASSIST:    Serial.print(F("Assist,  ")); break;
		case MAMODE1_STATE_IS_REGEN:     Serial.print(F("Regen,   ")); break;
		case MAMODE1_STATE_IS_IDLE:      Serial.print(F("Standby, ")); break;
		case MAMODE1_STATE_IS_AUTOSTOP:  Serial.print(F("AutoStop,")); break;
		case MAMODE1_STATE_IS_START:     Serial.print(F("Starting,")); break;
		case MAMODE1_STATE_IS_ERROR_HI:  Serial.print(F("Line HI, ")); break;
		case MAMODE1_STATE_IS_UNDEFINED: Serial.print(F("Error,  ")); break;
	}

	Serial.print(F(" CMDPWR:"));
	Serial.print( ecm_getCMDPWR_percent() );
	Serial.print('%');
}

/////////////////////////////////////////////////////////////////////////////////////////////

//Sending more than 63 characters per call makes this function blocking (until the buffer empties)!
void debugUSB_printLatestData(void)
{	
	static uint32_t previousMillis = 0;

	//print message if it's time and there's room in the serial transmit buffer
	if( (millis() - previousMillis) >= debugUSB_dataUpdatePeriod_ms_get() && (Serial.availableForWrite() > 62) )
	{
		previousMillis = millis();

		if     (debugUSB_dataTypeToStream_get() == DEBUGUSB_STREAM_BUTTON     ) { debugUSB_printButtonStates(); }
		else if(debugUSB_dataTypeToStream_get() == DEBUGUSB_STREAM_OEM_SIGNALS) { debugUSB_printOEMsignals();   }
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////