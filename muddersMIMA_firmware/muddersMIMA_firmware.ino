//Copyright 2022(c) John Sullivan

//Linsight MIMA firmware

#include "muddersMIMA.h"

//JTS2doNow: Pushing momentary button when key 'ON' - but not yet 'START' - enables DCDC by sending 'idle' signal on MAMODE (instead of 'prestart')

void setup()  
{
	gpio_begin();
	Serial.begin(115200); //USB

	Serial.print(F("\n\nWelcome to muddersMIMA v" FW_VERSION ", " BUILD_DATE "\nType '$HELP' for more info\n"));
}

void loop()
{
	static uint8_t toggleState_previous = TOGGLE_UNDEFINED;
	uint8_t toggleState = gpio_getButton_toggle();

	blinkLED();

	debugUSB_printLatestData();

	USB_userInterface_handler();

	//OEM IMA mode
	if(toggleState == TOGGLE_POSITION0)
	{
		//pass ECM signals unmodified to MCM
		mcm_setMAMODE1_state  (ecm_getMAMODE1_state()  );
		mcm_setMAMODE2_state  (ecm_getMAMODE2_state()  );
		mcm_setCMDPWR_percent (ecm_getCMDPWR_percent() );
	}

	//manual IMA mode (old)
	else if(toggleState == TOGGLE_POSITION1)
	{
		uint16_t joystick_counts = adc_readJoystick_rawCounts();

		if     (joystick_counts < JOYSTICK_MIN_COUNTS)         { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_COUNTS); } //signal too low
		else if(joystick_counts < JOYSTICK_NEUTRAL_MIN_COUNTS) { mcm_setAllSignals(MAMODE1_STATE_IS_REGEN,  joystick_counts);             } //manual regen
		else if(joystick_counts < JOYSTICK_NEUTRAL_MAX_COUNTS) { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   joystick_counts);             } //standby
		else if(joystick_counts < JOYSTICK_MAX_COUNTS)         { mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, joystick_counts);             } //manual assist
		else                                                   { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_COUNTS); } //signal too high
	}

	//PHEV mode (disable regen)
	// else if(toggleState == TOGGLE_POSITION2)
	// {
	// 	//PHEV mode (disable regen)
	// 	if(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN)
	// 	{
	// 		//we don't want regen, so ignore request
	// 		mcm_setAllSignals(MAMODE1_STATE_IS_IDLE, JOYSTICK_NEUTRAL_NOM_COUNTS);
	// 	}
	// 	else //(ECM not requesting regen)
	// 	{
	// 		//pass ECM signals unmodified to MCM
	// 		mcm_setMAMODE1_state  (ecm_getMAMODE1_state()  );
	// 		//mcm_setMAMODE1_percent(ecm_getMAMODE1_percent());
	// 		mcm_setMAMODE2_state  (ecm_getMAMODE2_state()  );
	// 		mcm_setCMDPWR_percent (ecm_getCMDPWR_percent() );
	// 	}

	//manual IMA mode (new)
	//JTS2doNow: allow OEM assist for ten seconds after clutch is let out
	else if(toggleState == TOGGLE_POSITION2)
	{
		static uint16_t joystick_counts_stored = JOYSTICK_NEUTRAL_NOM_COUNTS;
		static bool useStoredJoystickValue = NO;

		if(toggleState != toggleState_previous)
		{
			//clear previously stored joystick value (from the last time we were in manual mode)
			joystick_counts_stored = JOYSTICK_NEUTRAL_NOM_COUNTS;
			useStoredJoystickValue = NO;
		}

		if( (ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN ) ||
			(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_IDLE  ) ||
			(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_ASSIST)  )
		{
			//ECM is sending assist, idle, or regen signal...
			//but we're in manual mode, so use joystick value instead (either previously stored or value right now)

			uint16_t joystick_counts = adc_readJoystick_rawCounts(); //read joystick value

			if(gpio_getButton_momentary() == BUTTON_PRESSED)
			{
				//store joystick value when button is pressed
				joystick_counts_stored = joystick_counts;
				useStoredJoystickValue = YES;
			}

			//disable stored joystick value if regen too high (i.e. hard braking)
			if(ecm_getCMDPWR_percent() < 30)
			{
				useStoredJoystickValue = NO;
				joystick_counts_stored = JOYSTICK_NEUTRAL_NOM_COUNTS;
			} 

			//use stored joystick value if conditions are right
			if( (useStoredJoystickValue == YES                ) && //user previously pushed button
				(joystick_counts > JOYSTICK_NEUTRAL_MIN_COUNTS) && //joystick is neutral
				(joystick_counts < JOYSTICK_NEUTRAL_MAX_COUNTS)  ) //joystick is neutral
			{
				//replace actual joystick position with previously stored value
				joystick_counts = joystick_counts_stored;
			}
			
			//send assist/idle/regen value to MCM
			if     (joystick_counts < JOYSTICK_MIN_COUNTS)         { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_COUNTS); } //signal too low
			else if(joystick_counts < JOYSTICK_NEUTRAL_MIN_COUNTS) { mcm_setAllSignals(MAMODE1_STATE_IS_REGEN,  joystick_counts);             } //manual regen
			else if(joystick_counts < JOYSTICK_NEUTRAL_MAX_COUNTS) { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   joystick_counts);             } //standby
			else if(joystick_counts < JOYSTICK_MAX_COUNTS)         { mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, joystick_counts);             } //manual assist
			else                                                   { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_COUNTS); } //signal too high

		}
		else //ECM is sending autostop, prestart, or start signal
		{
			//pass these signals through unmodified (so autostop works properly)
			mcm_setMAMODE1_state  (ecm_getMAMODE1_state()  );
			mcm_setMAMODE2_state  (ecm_getMAMODE2_state()  );
			mcm_setCMDPWR_percent (ecm_getCMDPWR_percent() );

			//clear stored assist/idle/regen setpoint
			joystick_counts_stored = JOYSTICK_NEUTRAL_NOM_COUNTS;
			useStoredJoystickValue = NO;
		}

	}

	toggleState_previous = toggleState;

	time_waitForLoopPeriod(); //wait here until next iteration
}