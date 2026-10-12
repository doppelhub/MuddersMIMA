//Copyright 2022-2023(c) John Sullivan

//Send MCM control data, depending on IMA mode

#include "muddersMIMA.h"

uint8_t previous_assist = 50;
uint8_t previous_MAP = 50;
uint8_t previous_TPS = 0;
uint8_t assist_demand = 50;

/////////////////////////////////////////////////////////////////////////////////////////////

void mode_OEM(void) ///regen only
{
	brakeLights_setControlMode(BRAKE_LIGHT_MONITOR_ONLY);
	uint8_t latestVehicleMPH = engineSignals_getLatestVehicleMPH();
	uint16_t latestVehicleRPM = engineSignals_getLatestRPM();
	uint8_t regen_demand = 50-(sqrt(latestVehicleRPM-minrpm)*sqrt(latestVehicleMPH)/regenfactor);
	uint8_t regen_max = max(10,regen_demand);
		if 		(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_ASSIST) 	{ mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, 50); }
		else if	(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_IDLE)   	{ mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, 50); }
		else if	((ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN) &&  
				(gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_OFF)) { mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, 50); }
		else if	((ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN) && 
		        (gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_ON))	{ mcm_setAllSignals(MAMODE1_STATE_IS_REGEN, regen_max); }
		else /* (ECM requesting everyting else) */                		{ mcm_passUnmodifiedSignals_fromECM(); } 		
}


/////////////////////////////////////////////////////////////////////////////////////////////

//Pedal Controlled Proportional Assist.
//Automatic assist/regen using pedal position and engine signals
//(No joystick required)
//requires power user cable from Licontrol to vehicle ECU signals

void mode_proportional_auto_assist(void) // full power mode
{
	brakeLights_setControlMode(BRAKE_LIGHT_MONITOR_ONLY);
	uint8_t latestVehicleMPH = engineSignals_getLatestVehicleMPH();
	uint8_t TPS_percent = adc_getECM_TPS_percent()-tpsoffset; 
	uint8_t MAP_sensor = adc_getECM_MAP_percent(); 
	uint16_t latestVehicleRPM = engineSignals_getLatestRPM();
	
	if (latestVehicleMPH > maxmph) {latestVehicleMPH = 1;}  //safeguard
	
	 if      	(MAP_sensor > previous_MAP) { MAP_sensor = previous_MAP + 1; } 		// smooths engine load increase.
     else if    (MAP_sensor < previous_MAP) { MAP_sensor = previous_MAP - 1; } 		// smooths engine load decrease.
	 previous_MAP = MAP_sensor;	
	
	 if      	(latestVehicleMPH < 5) { TPS_percent = TPS_percent / 2; } 			//launch control (clutch slip)
	 else if    (latestVehicleMPH < 15) { TPS_percent = TPS_percent * 3/2; } 		//launch control (wheel slip)
	 if      	(TPS_percent > previous_TPS) { TPS_percent = previous_TPS + 1; } 	//smooths rapid accelerator pedal application
	 previous_TPS = TPS_percent;	
	
	uint8_t regen_demand = 50-(sqrt(latestVehicleRPM-minrpm)*sqrt(latestVehicleMPH)/regenfactor);
	uint8_t regen_max = max(10,regen_demand);
	
	//reduce low SOC assist	
	if (mima_battery_soc > 20) 		{ assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor)); } 
	else if (mima_battery_soc > 19) { assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor/2)); } 
	else if (mima_battery_soc > 18) { assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor/3)); } 
	else if (mima_battery_soc > 17) { assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor/4)); } 
	else if (mima_battery_soc > 16) { assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor/5)); } 
	else if (mima_battery_soc > 15) { assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor/6)); } 
	else {assist_demand = (50); } 
	
	uint8_t assist = min(90,assist_demand);
	
	 
	 if    		(assist > previous_assist) { assist = previous_assist + 1; } 		//prevents p1440 caused by rapid increase of assist demand
     else if    (assist < previous_assist) { assist = previous_assist - 1; } 		//prevents p1440 caused by rapid decrease of assist demand
	 previous_assist = assist;
	
	if 		    (ecm_getMAMODE1_state() == MAMODE1_STATE_IS_ASSIST)    	{ mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, assist); }
		else if	(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_IDLE)   	{ mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, assist); }
		else if	((ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN) &&  
				(gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_OFF)) { mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, 50); }
		else if	((ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN) && 
		        (gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_ON))	{ mcm_setAllSignals(MAMODE1_STATE_IS_REGEN, regen_max); }
		else /* (ECM requesting everyting else) */                		{ mcm_passUnmodifiedSignals_fromECM(); } 
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


//low power VERSION
void mode_backup(void)
{
	brakeLights_setControlMode(BRAKE_LIGHT_MONITOR_ONLY);
	uint8_t latestVehicleMPH = engineSignals_getLatestVehicleMPH();
	uint8_t TPS_percent = adc_getECM_TPS_percent()-tpsoffset; 
	uint8_t MAP_sensor = adc_getECM_MAP_percent(); 
	uint16_t latestVehicleRPM = engineSignals_getLatestRPM();
	
	if (latestVehicleMPH > maxmph) {latestVehicleMPH = 1;}  //safeguard
	
	 if      	(MAP_sensor > previous_MAP) { MAP_sensor = previous_MAP + 1; } 		// smooths engine load increase.
     else if    (MAP_sensor < previous_MAP) { MAP_sensor = previous_MAP - 1; } 		// smooths engine load decrease.
	 previous_MAP = MAP_sensor;	
	

	 if      	(latestVehicleMPH < 5) { TPS_percent = TPS_percent / 2; } 			//launch control (clutch slip)
	 else if    (latestVehicleMPH < 15) { TPS_percent = TPS_percent * 3/2; } 		//launch control (wheel slip)
	 if      	(TPS_percent > previous_TPS) { TPS_percent = previous_TPS + 1; } 	//smooths rapid accelerator pedal application
	 previous_TPS = TPS_percent;	
	
	
	uint8_t regen_demand = 50-(sqrt(latestVehicleRPM-minrpm)*sqrt(latestVehicleMPH)/regenfactor);
	uint8_t regen_max = max(10,regen_demand);
	 
	//reduce low SOC assist	
	if (mima_battery_soc > 20) 		{ assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor/2)); } 
	else if (mima_battery_soc > 19) { assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor/3)); } 
	else if (mima_battery_soc > 18) { assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor/4)); } 
	else if (mima_battery_soc > 17) { assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor/5)); } 
	else if (mima_battery_soc > 16) { assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor/6)); } 
	else if (mima_battery_soc > 15) { assist_demand = (50+(sqrt(latestVehicleMPH)*sqrt(TPS_percent)*sqrt(MAP_sensor)/assistfactor/7)); } 
	else {assist_demand = (50); } 
	
	uint8_t assist = min(90,assist_demand);
	
	 
	 if      	(assist > previous_assist) { assist = previous_assist + 1; } 		//prevents p1440 caused by rapid increase of assist demand
     else if    (assist < previous_assist) { assist = previous_assist - 1; } 		//prevents p1440 caused by rapid decrease of assist demand
	 previous_assist = assist;
	
	if 		    (ecm_getMAMODE1_state() == MAMODE1_STATE_IS_ASSIST)    { mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, assist); }
		else if	(ecm_getMAMODE1_state() == MAMODE1_STATE_IS_IDLE)   	{ mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, assist); }
		else if	((ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN) &&  
				(gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_OFF)) { mcm_setAllSignals(MAMODE1_STATE_IS_ASSIST, 50); }
		else if	((ecm_getMAMODE1_state() == MAMODE1_STATE_IS_REGEN) && 
		        (gpio_getBrakePosition_bool() == BRAKE_LIGHTS_ARE_ON))	{ mcm_setAllSignals(MAMODE1_STATE_IS_REGEN, regen_max); }
		else /* (ECM requesting everyting else) */                		{ mcm_passUnmodifiedSignals_fromECM(); } 
	
}

/////////////////////////////////////////////////////////////////////////////////////////////


void operatingModes_handler(void)
{
	uint8_t toggleState = gpio_getButton_toggle();
	static uint8_t toggleState_previous = TOGGLE_UNDEFINED;

	if     (toggleState == TOGGLE_POSITION0) { MODE0_BEHAVIOR(); } //see #define substitutions in config.h
	else if(toggleState == TOGGLE_POSITION1) { MODE1_BEHAVIOR(); }
	else if(toggleState == TOGGLE_POSITION2) { MODE2_BEHAVIOR(); }
	else /* hidden 'mode3' (unsupported) */  { MODE0_BEHAVIOR(); }

	toggleState_previous = toggleState;
}
