//Copyright 2022(c) John Sullivan

//Linsight MIMA firmware

#include "muddersMIMA.h"

//JTS2doNow: Make Bootloader boot faster (only load firmware if button held down when keyON)

void setup()  
{
	gpio_begin();
	Serial.begin(115200); //USB

	Serial.print(F("\n\nWelcome to muddersMIMA v" FW_VERSION ", " BUILD_DATE "\nType '$HELP' for more info\n"));

	gpio_turnBuzzer_on();
	delay(1000);
	gpio_turnBuzzer_off();
}

void loop()
{
	blinkLED();

	debugUSB_printLatestData();

	USB_userInterface_handler();

	//pass ECM signals unmodified to MCM
	//This will be "mode 0" initially (so I can turn system off if needed)
	mcm_setMAMODE1_state  (ecm_getMAMODE1_state()  );
	//mcm_setMAMODE1_percent(ecm_getMAMODE1_percent());
	mcm_setMAMODE2_state  (ecm_getMAMODE2_state()  );
	mcm_setCMDPWR_percent (ecm_getCMDPWR_percent() );

	time_waitForLoopPeriod(); //wait here until next iteration
}