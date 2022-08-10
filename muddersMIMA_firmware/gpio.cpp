//Copyright 2022(c) John Sullivan


//all digitalRead(), digitalWrite(), analogRead(), analogWrite() functions live here
//JTS2doLater: Replace Arduino fcns with low level

#include "muddersMIMA.h"

////////////////////////////////////////////////////////////////////////////////////

void gpio_begin(void)
{
	pinMode(PIN_BUZZER,OUTPUT);
	pinMode(PIN_CMDPWR_MCM,OUTPUT);

	pinMode(PIN_MAMODE1_MCM,OUTPUT);
	pinMode(PIN_MAMODE2_MCM,OUTPUT);
	pinMode(PIN_LED_UNO,OUTPUT);

	pinMode(PIN_USER_MOMENTARY, INPUT_PULLUP);
	pinMode(PIN_USER_TOGGLE1, INPUT_PULLUP);
	pinMode(PIN_USER_TOGGLE2, INPUT_PULLUP);

	//TCCR1B = (TCCR1B & B11111000) | B00000001; // Set F_PWM to 31372.55 Hz //pins D11(fan) & D12()
	//TCCR3B = (TCCR3B & B11111000) | B00000001; // Set F_PWM to 31372.55 Hz //pins D2() & D3() & D5(VPIN_OUT)
	//TCCR4B = (TCCR4B & B11111000) | B00000010; // Set F_PWM to  3921.16 Hz //pins D7(MCMe) & D8(gridPWM) & D9()
}

////////////////////////////////////////////////////////////////////////////////////

bool gpio_getButton_momentary(void) { return digitalRead(PIN_USER_MOMENTARY); }

////////////////////////////////////////////////////////////////////////////////////

uint8_t gpio_getButton_toggle(void)
{
	uint8_t toggleState = digitalRead(PIN_USER_TOGGLE1); //set 0b0000 000n
	toggleState |= (digitalRead(PIN_USER_TOGGLE2) << 1); //set 0b0000 00nx

	return toggleState;
}

////////////////////////////////////////////////////////////////////////////////////

void gpio_turnBuzzer_on(void)  { analogWrite(PIN_BUZZER, 127 ); }
void gpio_turnBuzzer_off(void) { analogWrite(PIN_BUZZER, 0); }

////////////////////////////////////////////////////////////////////////////////////

uint16_t gpio_getMAMODE1_counts(void) { return analogRead(PIN_MAMODE1_ECM); }
uint16_t gpio_getCMDPWR_counts(void)  { return analogRead(PIN_CMDPWR_ECM); }

////////////////////////////////////////////////////////////////////////////////////

bool gpio_getMAMODE2(void) { return digitalRead(PIN_MAMODE2_ECM); }


////////////////////////////////////////////////////////////////////////////////////