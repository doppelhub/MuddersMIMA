//Copyright 2022(c) John Sullivan


//debug stuff

#include "muddersMIMA.h"

/////////////////////////////////////////////////////////////////////////////////////////////

void debugLED(bool illuminated)
{
	#ifdef LED_DEBUG
		digitalWrite(PIN_LED_UNO,illuminated);
	#else
		illuminated +=0; //prevent "unused parameter" compiler warning
	#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LED(bool illuminated)
{
	#ifdef LED_NORMAL
		digitalWrite(PIN_LED_UNO,illuminated);
	#else
		illuminated +=0; //prevent "unused parameter" compiler warning
	#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////

void blinkLED()
{
	#ifdef LED_NORMAL
		static uint32_t previousMillis = 0;
		if(millis() - previousMillis >= 100)
		{
			previousMillis = millis();
			digitalWrite(PIN_LED_UNO, !digitalRead(PIN_LED_UNO) );
		}
	#endif	
}