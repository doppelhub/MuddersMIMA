//Copyright 2022(c) John Sullivan

//Linsight MIMA firmware

#include "muddersMIMA.h"

//JTS2doNow: Make Bootloader boot faster (only load firmware if button held down when keyON)

void setup()  
{
	gpio_begin();
	Serial.begin(115200); //USB

	Serial.print(F("\n\nWelcome to muddersMIMA v" FW_VERSION ", " BUILD_DATE "\nType '$HELP' for more info\n"));
}

void loop()
{
	blinkLED();

	debugUSB_printLatestData();

	USB_userInterface_handler();

	if(gpio_getButton_toggle() == TOGGLE_POSITION0)
	{
		//pass ECM signals unmodified to MCM
		mcm_setMAMODE1_state  (ecm_getMAMODE1_state()  );
		//mcm_setMAMODE1_percent(ecm_getMAMODE1_percent());
		mcm_setMAMODE2_state  (ecm_getMAMODE2_state()  );
		mcm_setCMDPWR_percent (ecm_getCMDPWR_percent() );
	}
	else if(gpio_getButton_toggle() == TOGGLE_POSITION1)
	{
		//manual IMA mode
		uint16_t joystick_counts = adc_readJoystick_rawCounts();

		if     (joystick_counts < JOYSTICK_MIN_COUNTS)         { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_COUNTS); } //signal too low
		else if(joystick_counts < JOYSTICK_NEUTRAL_MIN_COUNTS) { mcm_setAllSignals(MAMODE1_STATE_IS_REGEN,  joystick_counts);             } //manual regen
		else if(joystick_counts < JOYSTICK_NEUTRAL_MAX_COUNTS) { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   joystick_counts);             } //standby
		else if(joystick_counts < JOYSTICK_MAX_COUNTS)         { mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, joystick_counts);             } //manual assist
		else                                                   { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_COUNTS); } //signal too high
	}
	else if(gpio_getButton_toggle() == TOGGLE_POSITION2)
	{
		//Disable Regen mode
		if(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN)
		{
			//we don't want regen, so ignore request
			mcm_setAllSignals(MAMODE1_STATE_IS_IDLE, JOYSTICK_NEUTRAL_NOM_COUNTS);
		}
		else //(ECM not requesting regen)
		{
			//pass ECM signals unmodified to MCM
			mcm_setMAMODE1_state  (ecm_getMAMODE1_state()  );
			//mcm_setMAMODE1_percent(ecm_getMAMODE1_percent());
			mcm_setMAMODE2_state  (ecm_getMAMODE2_state()  );
			mcm_setCMDPWR_percent (ecm_getCMDPWR_percent() );
		}
	}

	time_waitForLoopPeriod(); //wait here until next iteration
}