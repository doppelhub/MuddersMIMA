//Copyright 2022-2023(c) John Sullivan

//LiControl manual IMA Controller firmware

#include "muddersMIMA.h"

//To update the firmware:
	//1 : Open this file in the Arduino application
	//2 : Plug the LiControl USB cable into your computer.
	//3 : In the 'Tools' menu, select "Board:_________" -> "Arduino AVR Boards" -> "Arduino Nano"
	//4 : In the 'Tools' menu, select "Port" -> <<your device's COM port>>
	//5a: Push the momentary button down with your finger
	//5b: Keep holding this button down until the upload process completes
	//6 : Click the upload button
	//7 : Wait for the upload to complete
	//8 : Release the momentary button

void setup()  
{
	gpio_begin();
	Serial.begin(115200); //USB
	Serial.print(F("\n\nWelcome to LiControl v" FW_VERSION ", " BUILD_DATE "\nType '$HELP' for more info\n"));
}

void loop()
{
	ecm_handler();
	time_handler();
	operatingModes_handler();
	USB_userInterface_handler();
	
	debugUSB_printLatestData();

	time_waitForLoopPeriod(); //wait here until next iteration
}