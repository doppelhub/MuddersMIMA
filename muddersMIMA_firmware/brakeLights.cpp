//Copyright 2022-2023(c) John Sullivan

#include "muddersMIMA.h"

////////////////////////////////////////////////////////////////////////////////////

// //Control logic is something like
// if( (gpio_brakeLightStatus() == BRAKE_LIGHTS_ARE_ON) &&
//     (doesLiControlWantBrakeLightsOn() == NO        )  )	
// {	
// 	pinMode(PIN_BRAKE_uC,OUTPUT); //gpio_turnBrakeLightsOff()
// 	digitalWrite(PIN_BRAKE_uC,LOW);
// }
// else if( (gpio_brakeLightStatus() == BRAKE_LIGHTS_ARE_OFF) &&		
//          (doesLiControlWantBrakeLightsOn() == YES        )  )
// {
// 		pinMode(PIN_BRAKE_uC,OUTPUT); //gpio_turnBrakeLightsOn()
// 		digitalWrite(PIN_BRAKE_uC,HIGH);
// }	
// else
// {
// 	pinMode(PIN_BRAKE_uC,INPUT);
// 	digitalWrite(PIN_BRAKE_uC,LOW);
// }