//Copyright 2022-2023(c) John Sullivan

//Send MCM control data, depending on IMA mode

#include "muddersMIMA.h"

uint8_t joystick_percent_stored = JOYSTICK_NEUTRAL_NOM_PERCENT;
bool useStoredJoystickValue = NO; //JTS2doLater: I'm not convinced this is required

/////////////////////////////////////////////////////////////////////////////////////////////

void mode_OEM(void)
{
	brakeLights_setControlMode(BRAKE_LIGHT_OEM); 
	mcm_passUnmodifiedSignals_fromECM();
}

/////////////////////////////////////////////////////////////////////////////////////////////

//PHEV mode
//JTS2doNow: implement manual regen
void mode_INWORK_manualRegen_autoAssist(void)
{
	brakeLights_setControlMode(BRAKE_LIGHT_OEM);

	if(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN) { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE, JOYSTICK_NEUTRAL_NOM_PERCENT); } //ignore regen request
	else /* (ECM not requesting regen) */                { mcm_passUnmodifiedSignals_fromECM(); } //pass all other signals through
}

/////////////////////////////////////////////////////////////////////////////////////////////

//LiControl completely ignores ECM signals (including autostop, autostart, prestart, etc)
void mode_manualAssistRegen_ignoreECM(void)
{
	brakeLights_setControlMode(BRAKE_LIGHT_AUTOMATIC);

	uint16_t joystick_percent = adc_readJoystick_percent();

	if     (joystick_percent < JOYSTICK_MIN_ALLOWED_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_PERCENT); } //signal too low
	else if(joystick_percent < JOYSTICK_NEUTRAL_MIN_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_REGEN,  joystick_percent);             } //manual regen
	else if(joystick_percent < JOYSTICK_NEUTRAL_MAX_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   joystick_percent);             } //standby
	else if(joystick_percent < JOYSTICK_MAX_ALLOWED_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, joystick_percent);             } //manual assist
	else                                                     { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_PERCENT); } //signal too high
}

/////////////////////////////////////////////////////////////////////////////////////////////

void mode_manualAssistRegen_withAutoStartStop(void)
{
	brakeLights_setControlMode(BRAKE_LIGHT_AUTOMATIC);

	if( (ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN ) ||
		(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_IDLE  ) ||
		(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_ASSIST)  )
	{
		//ECM is sending assist, idle, or regen signal...
		//but we're in manual mode, so use joystick value instead (either previously stored or value right now)

		uint16_t joystick_percent = adc_readJoystick_percent();

		if(gpio_getButton_momentary() == BUTTON_PRESSED)
		{
			//store joystick value when button is pressed
			joystick_percent_stored = joystick_percent;
			useStoredJoystickValue = YES;
		}

		//disable stored joystick value if user is braking
		//JTS2doLater: Add clutch disable
		if(gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_ON)
		{
			useStoredJoystickValue = NO;
			joystick_percent_stored = JOYSTICK_NEUTRAL_NOM_PERCENT;
		} 

		//use stored joystick value if conditions are right
		if( (useStoredJoystickValue == YES                ) && //user previously pushed button
			(joystick_percent > JOYSTICK_NEUTRAL_MIN_PERCENT) && //joystick is neutral
			(joystick_percent < JOYSTICK_NEUTRAL_MAX_PERCENT)  ) //joystick is neutral
		{
			//replace actual joystick position with previously stored value
			joystick_percent = joystick_percent_stored;
		}
		
		//send assist/idle/regen value to MCM
		if     (joystick_percent < JOYSTICK_MIN_ALLOWED_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_PERCENT); } //signal too low
		else if(joystick_percent < JOYSTICK_NEUTRAL_MIN_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_REGEN,  joystick_percent);             } //manual regen
		else if(joystick_percent < JOYSTICK_NEUTRAL_MAX_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   joystick_percent);             } //standby
		else if(joystick_percent < JOYSTICK_MAX_ALLOWED_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, joystick_percent);             } //manual assist
		else                                                     { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_PERCENT); } //signal too high

	}
	else if(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_PRESTART)
	{
		//prevent DCDC disable when user regen-stalls car

		//DCDC converter must be disabled when the key first turns on.
		//Otherwise, the DCDC input current prevents the HVDC capacitors from pre-charging through the precharge resistor.
		//This can cause intermittent P1445, particularly after rapidly turning key off and on.

		//Therefore, we need to honor the ECM's PRESTART request for the first few seconds after keyON (so the HVDC bus voltage can charge to the pack voltage).

		//JTS2doNow: if SoC too low (get from LiBCM), pass through unmodified signal (which will disable DCDC) 
		if(millis() < (time_latestKeyOn_ms() + PERIOD_AFTER_KEYON_WHERE_PRESTART_ALLOWED_ms)) { mcm_passUnmodifiedSignals_fromECM(); } //key hasn't been on long enough
		else { mcm_setAllSignals(MAMODE1_STATE_IS_AUTOSTOP, JOYSTICK_NEUTRAL_NOM_PERCENT); } //JTS2doLater: This prevents user from manually assist-starting IMA

		//clear stored assist/idle/regen setpoint
		joystick_percent_stored = JOYSTICK_NEUTRAL_NOM_PERCENT;
		useStoredJoystickValue = NO;
	}
	else //ECM is sending autostop, start, or undefined signal
	{
		//pass these signals through unmodified (so autostop works properly)
		mcm_passUnmodifiedSignals_fromECM();

		//clear stored assist/idle/regen setpoint
		joystick_percent_stored = JOYSTICK_NEUTRAL_NOM_PERCENT;
		useStoredJoystickValue = NO;
	}

	//JTS2doLater: New feature: When the key is on and the engine is off, pushing momentary button starts engine.
}

/////////////////////////////////////////////////////////////////////////////////////////////

//GOAL: All OEM signals are passed through unmodified, except:
//CMDPWR assist
	//LiControl uses strongest assist request (user or ECM), except that;
	//pressing the momentary button stores the joystick position (technically the value is stored on button release)
	//after pressing the momentary button, all ECM assist requests are ignored until the user either brakes or (temporarily) changes modes   
	//manual joystick assist requests are allowed even after pushing momentary button; stored value resumes once joystick is neutral again
//CMDPWR regen
	//LiControl ignores ECM regen requests, unless user is braking
	//when braking and joystick is neutral, LiControl uses ECM regen request
	//manual joystick regen request always overrides ECM regen request
//MAMODE1 prestart
	//modified to always enable DCDC when key is on
void mode_INWORK_PHEV_mudder(void)
{
	brakeLights_setControlMode(BRAKE_LIGHT_MONITOR_ONLY); //JTS2doLater: if possible, add strong regen brake lights

	if( (ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN ) ||
		(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_IDLE  ) ||
		(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_ASSIST)  )
	{
		//ECM is sending assist, idle, or regen signal

		uint8_t joystick_percent = adc_readJoystick_percent();
		uint8_t ECM_CMDPWR_percent = ecm_getCMDPWR_percent();

		if (ECM_CMDPWR_percent > joystick_percent) { joystick_percent = ECM_CMDPWR_percent; } //choose strongest assist request (user or ECM)

		if(gpio_getButton_momentary() == BUTTON_PRESSED)
		{
			//store joystick value when button is pressed
			joystick_percent_stored = joystick_percent;
			useStoredJoystickValue = YES;
		}

		//disable stored joystick value if user is braking
		//JTS2doLater: Add clutch disable
		if(gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_ON)
		{
			useStoredJoystickValue = NO;
			joystick_percent_stored = JOYSTICK_NEUTRAL_NOM_PERCENT;	
		} 

		//Use ECM regen request when user is braking AND joystick is neutral
		if ((joystick_percent > JOYSTICK_NEUTRAL_MIN_PERCENT)     && //joystick is neutral
			(joystick_percent < JOYSTICK_NEUTRAL_MAX_PERCENT)     && //joystick is neutral
			(gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_ON)  ) //user is braking
		{
			//while braking, replace neutral joystick position with ECM regen request
			joystick_percent = ecm_getCMDPWR_percent();
		}

		//use stored joystick value if conditions are right
		if( (useStoredJoystickValue == YES                  ) && //user previously pushed button
			(joystick_percent > JOYSTICK_NEUTRAL_MIN_PERCENT) && //joystick is neutral
			(joystick_percent < JOYSTICK_NEUTRAL_MAX_PERCENT)  ) //joystick is neutral
		{
			//replace actual joystick position with previously stored value
			joystick_percent = joystick_percent_stored;
		}
		
		//send assist/idle/regen value to MCM
		if     (joystick_percent < JOYSTICK_MIN_ALLOWED_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_PERCENT); } //signal too low
		else if(joystick_percent < JOYSTICK_NEUTRAL_MIN_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_REGEN,  joystick_percent);             } //manual regen
		else if(joystick_percent < JOYSTICK_NEUTRAL_MAX_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   joystick_percent);             } //standby
		else if(joystick_percent < JOYSTICK_MAX_ALLOWED_PERCENT) { mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, joystick_percent);             } //manual assist
		else                                                     { mcm_setAllSignals(MAMODE1_STATE_IS_IDLE,   JOYSTICK_NEUTRAL_NOM_PERCENT); } //signal too high

	}
	else if(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_PRESTART)
	{
		//prevent DCDC disable when user regen-stalls car

		//DCDC converter must be disabled when the key first turns on.
		//Otherwise, the DCDC input current prevents the HVDC capacitors from pre-charging through the precharge resistor.
		//This can cause intermittent P1445, particularly after rapidly turning key off and on.

		//Therefore, we need to honor the ECM's PRESTART request for the first few seconds after keyON (so the HVDC bus voltage can charge to the pack voltage).

		//JTS2doNow: if SoC too low (get from LiBCM), pass through unmodified signal (which will disable DCDC) 
		if(millis() < (time_latestKeyOn_ms() + PERIOD_AFTER_KEYON_WHERE_PRESTART_ALLOWED_ms)) { mcm_passUnmodifiedSignals_fromECM(); } //key hasn't been on long enough
		else { mcm_setAllSignals(MAMODE1_STATE_IS_AUTOSTOP, JOYSTICK_NEUTRAL_NOM_PERCENT); } //JTS2doLater: This prevents user from manually assist-starting IMA

		//clear stored assist/idle/regen setpoint
		joystick_percent_stored = JOYSTICK_NEUTRAL_NOM_PERCENT;
		useStoredJoystickValue = NO;
	}
	else //ECM is sending autostop, start, or undefined signal
	{
		//pass these signals through unmodified (so autostop works properly)
		mcm_passUnmodifiedSignals_fromECM();

		//clear stored assist/idle/regen setpoint
		joystick_percent_stored = JOYSTICK_NEUTRAL_NOM_PERCENT;
		useStoredJoystickValue = NO;
	}

	//JTS2doLater: New feature: When the key is on and the engine is off, pushing momentary button starts engine.
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Pedal Controlled Proportional Assist.
//Provides assist by adding demand on top of the OEM IMA strategy
//Uses the OEM regen strategy
//Fully automatic operation (No joystick required)

void mode_proportional_auto_assist(void)
void mode_proportional_auto_assist(void)
{
	brakeLights_setControlMode(BRAKE_LIGHT_AUTOMATIC);
	uint8_t ECM_CMDPWR_percent = ecm_getCMDPWR_percent();
	uint8_t latestVehicleMPH = engineSignals_getLatestVehicleMPH();
	uint8_t TPS_percent = adc_getECM_TPS_percent()-9; //TPS offset
	uint8_t latestVehicleRPM = engineSignals_getLatestRPM();
	
	uint8_t Assist=.96; 	// light assist is reduced to compensate for increase in power levels due to voltage spoofing.
	uint8_t Boost=.15; 	// circa 10kW assist under gentle acceleration (full assist with larger throttle openings) 
	uint8_t Cruise=.35; 	// provides assist during cruise. (circa 6kW highway speeds)
	uint8_t Coast=1.04; 	// slight reduction in lift-off regen to make smoother (compensates for increase in power levels due to voltage spoofing).
	uint8_t Brake=.25;  	// increases brake regen to mask 3rd gear glitch & provide full regen under braking in all gears
				// recommended that background regen (part throttle & forced) are be disabled in LIBCM config (REDUCE_BACKGROUND_REGEN_UNLESS_BRAKING)

		if 	(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_ASSIST) 	{ mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, ((Assist*ECM_CMDPWR_percent)+(Boost*sqrt(latestVehicleMPH)*TPS_percent ))); } 		
		else if	(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_IDLE)   	{ mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, (ECM_CMDPWR_percent+(Cruise*sqrt(latestVehicleMPH)*TPS_percent ))); }
		else if	((ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN) &&  (gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_OFF)) 	{ mcm_setAllSignals(MAMODE1_STATE_IS_REGEN, (Coast*ECM_CMDPWR_percent)); }
		else if	(gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_ON)  	{ mcm_setAllSignals(MAMODE1_STATE_IS_REGEN, (ECM_CMDPWR_percent-(Brake*latestVehicleMPH))); } //TODO: disable this below ~1200rpm
		else /* (ECM requesting everyting else) */                	{ mcm_passUnmodifiedSignals_fromECM(); } 				

}

/////////////////////////////////////////////////////////////////////////////////////////////
void operatingModes_handler(void)
{
	uint8_t toggleState = gpio_getButton_toggle();
	static uint8_t toggleState_previous = TOGGLE_UNDEFINED;

	if(toggleState != toggleState_previous)
	{
		//clear previously stored joystick value (from the last time we were in manual mode)
		joystick_percent_stored = JOYSTICK_NEUTRAL_NOM_PERCENT;
		useStoredJoystickValue = NO;
	}

	if     (toggleState == TOGGLE_POSITION0) { MODE0_BEHAVIOR(); } //see #define substitutions in config.h
	else if(toggleState == TOGGLE_POSITION1) { MODE1_BEHAVIOR(); }
	else if(toggleState == TOGGLE_POSITION2) { MODE2_BEHAVIOR(); }
	else /* hidden 'mode3' (unsupported) */  { MODE0_BEHAVIOR(); }

	toggleState_previous = toggleState;
}
