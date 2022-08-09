//Copyright 2022(c) John Sullivan

//Linsight MIMA firmware

#include "muddersMIMA.h"

//JTS2doNow: Make Bootloader boot faster (only load firmware if button held down when keyON)

void setup()  
{
	gpio_begin();
	Serial.begin(115200); //USB

	Serial.print(F("\n\nWelcome to LiMIMA v" FW_VERSION ", " BUILD_DATE "\nType '$HELP' for more info\n"));

	gpio_turnBuzzer_on();
	delay(1000);
	gpio_turnBuzzer_off();
}

void loop()
{
	blinkLED();

	debugUSB_printButtonStates();

	USB_userInterface_handler();

	time_waitForLoopPeriod(); //wait here until next iteration
}