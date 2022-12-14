//Copyright 2022(c) John Sullivan


//all digitalRead(), digitalWrite(), analogRead(), analogWrite() functions live here
//JTS2doLater: Replace Arduino fcns with low level

#include "muddersMIMA.h"

////////////////////////////////////////////////////////////////////////////////////

void gpio_begin(void)
{
	pinMode(PIN_BUZZER,OUTPUT);

	pinMode(PIN_CMDPWR_MCM,OUTPUT);
	pinMode(PIN_CMDPWR_ECM,INPUT); //disables pullup... which causes RC LPF to read high

	pinMode(PIN_MAMODE1_MCM,OUTPUT);
	pinMode(PIN_MAMODE1_ECM,INPUT); //disables pullup... which causes RC LPF to read high

	pinMode(PIN_MAMODE2_MCM,OUTPUT);
	pinMode(PIN_MAMODE2_ECM,INPUT_PULLUP);
	
	pinMode(PIN_LED_UNO,OUTPUT);

	pinMode(PIN_USER_MOMENTARY, INPUT_PULLUP);
	pinMode(PIN_USER_TOGGLE1, INPUT_PULLUP);
	pinMode(PIN_USER_TOGGLE2, INPUT_PULLUP);

	analogWrite(PIN_CMDPWR_MCM, 511); //10b counter set to 50% PWM
    analogWrite(PIN_MAMODE1_MCM, 127); //8b counter set to 50% PWM
    digitalWrite(PIN_MAMODE2_MCM, MAMODE2_STATE_IS_REGEN_STANDBY);

    // Pins D9 and D10 @ 2 kHz
  	TCCR1A = 0b00000011; // 10bit
  	TCCR1B = 0b00001010; // x8 fast pwm

  	// Pins D3 and D11 - 31.4 kHz (OEM is 20 kHz, but this is close enough)
  	TCCR2B = 0b00000001; // x1 8bit
  	TCCR2A = 0b00000001; // phase correct
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

uint16_t gpio_getMAMODE1_counts(void)     { return analogRead(PIN_MAMODE1_ECM); } //10b ADC result //signal read from ECM
void     gpio_setMAMODE1_counts(uint8_t counts) { analogWrite(PIN_MAMODE1_MCM, counts); } //8b counter //signal sent to MCM

uint16_t gpio_getCMDPWR_counts(void)      { return analogRead(PIN_CMDPWR_ECM); } //10b ADC result //signal read from ECM
void     gpio_setCMDPWR_counts(uint16_t counts) { analogWrite(PIN_CMDPWR_MCM, counts); } //10b counter //signal sent to MCM

////////////////////////////////////////////////////////////////////////////////////

bool gpio_getMAMODE2(void) { return digitalRead(PIN_MAMODE2_ECM); } //signal read from ECM
void gpio_setMAMODE2(bool mode)  { digitalWrite(PIN_MAMODE2_MCM, mode); } //signal sent to MCM
////////////////////////////////////////////////////////////////////////////////////