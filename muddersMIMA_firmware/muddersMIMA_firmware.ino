//Copyright 2022-2023(c) John Sullivan

//LiControl manual IMA Controller firmware

//JTS2doLater: Convert tabs to 4 spaces once there aren't any open pull requests

#include "muddersMIMA.h"

//To update the firmware:
	//1 : Open this file in the Arduino application
	//2 : Plug the LiControl USB cable into your computer.
	//3 : In the 'Tools' menu, select "Board:_________" -> "Arduino AVR Boards" -> "Arduino Nano"
	//4a: In the 'Tools' menu, select "Port" -> <<your device's COM port>>
    //4b: If device not shown, install CH340 driver (learn.sparkfun.com/tutorials/how-to-install-ch340-drivers)
	//5a: Push the momentary button down with your finger
	//5b: Keep holding this button down until the upload process completes
	//6 : Click the upload button
	//7 : Wait for the upload to complete
	//8 : Release the momentary button

void setup()  
{
	gpio_begin();
	engineSignals_begin();
	Serial.begin(115200); //USB
	Serial.print(F("\n\nWelcome to LiControl v" FW_VERSION ", " BUILD_DATE "\nType '$HELP' for more info\n"));
}

void loop()
{
	adc_handler();
	ecm_handler();
	time_handler();
	brakeLights_handler();
	operatingModes_handler();
	USB_userInterface_handler();

	debugUSB_printLatestData();

	time_waitForLoopPeriod(); //wait here until next iteration
}