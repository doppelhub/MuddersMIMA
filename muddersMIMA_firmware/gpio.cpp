//Copyright 2022-2023(c) John Sullivan


//all digitalRead(), digitalWrite(), analogWrite() functions live here
//JTS2doLater: Replace Arduino fcns with low level

#include "muddersMIMA.h"

uint8_t mcmCMDPWR_Percent = 50;

////////////////////////////////////////////////////////////////////////////////////

void gpio_begin(void)
{
	pinMode(PIN_CMDPWR_MCM,OUTPUT);
	pinMode(PIN_CMDPWR_ECM,INPUT); //disables pullup... which causes RC LPF to read high

	pinMode(PIN_MAMODE1_MCM,OUTPUT);
	pinMode(PIN_MAMODE1_ECM,INPUT); //disables pullup... which causes RC LPF to read high

	pinMode(PIN_MAMODE2_MCM,OUTPUT);
	pinMode(PIN_MAMODE2_ECM,INPUT_PULLUP);

	pinMode(PIN_USER_MOMENTARY, INPUT_PULLUP);
	pinMode(PIN_USER_TOGGLE1, INPUT_PULLUP);
	pinMode(PIN_USER_TOGGLE2, INPUT_PULLUP);

	analogWrite(PIN_CMDPWR_MCM, 511); //10b counter set to 50% PWM
    analogWrite(PIN_MAMODE1_MCM, 127); //8b counter set to 50% PWM
    digitalWrite(PIN_MAMODE2_MCM, MAMODE2_STATE_IS_REGEN_STANDBY);

    // Pins D9 and D10 @ 2 kHz
  	//TCCR1A = 0b00000011; // 10bit
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

void gpio_setMCM_MAMODE1_percent(uint8_t newPercent)
{
	uint16_t counts = (uint16_t)newPercent * 2.55; //output PWM uses 8b counter (percent*255/100)

	if(counts > 255) { counts = 255; }

	analogWrite(PIN_MAMODE1_MCM, counts); //8b counter
}

////////////////////////////////////////////////////////////////////////////////////

void gpio_setMCM_CMDPWR_percent(uint8_t newPercent)
{
	mcmCMDPWR_Percent = newPercent;

	uint16_t counts = (uint16_t)newPercent * 2.55; //output PWM uses 8b counter (percent*255/100)

	if(counts > 255) { counts = 255; }

	analogWrite(PIN_CMDPWR_MCM, counts); //8b counter
}

////////////////////////////////////////////////////////////////////////////////////

uint8_t gpio_getMCM_CMDPWR_percent(void) { return mcmCMDPWR_Percent; }

////////////////////////////////////////////////////////////////////////////////////

bool gpio_getECM_MAMODE2_bool(void) { return digitalRead(PIN_MAMODE2_ECM); } //signal read from ECM
void gpio_setMCM_MAMODE2_bool(bool mode) { digitalWrite(PIN_MAMODE2_MCM, mode); } //signal sent to MCM

////////////////////////////////////////////////////////////////////////////////////

bool gpio_getBrakePosition_bool(void)
{
	if(digitalRead(PIN_BRAKE) == LOW) { return BRAKE_LIGHTS_ARE_OFF; }
	else                              { return BRAKE_LIGHTS_ARE_ON;  }
}

////////////////////////////////////////////////////////////////////////////////////

void gpio_brakeLights_turnOn(void)
{
	pinMode(PIN_BRAKE,OUTPUT);
	digitalWrite(PIN_BRAKE,HIGH);
}

////////////////////////////////////////////////////////////////////////////////////

void gpio_brakeLights_turnOff(void)
{
	pinMode(PIN_BRAKE,OUTPUT);
	digitalWrite(PIN_BRAKE,LOW);
}

////////////////////////////////////////////////////////////////////////////////////

void gpio_brakeLights_floatPin(void)
{
	pinMode(PIN_BRAKE,INPUT);
}

////////////////////////////////////////////////////////////////////////////////////

bool gpio_getClutchPosition(void)
{
	if(digitalRead(PIN_CLUTCH) == LOW) { return CLUTCH_PEDAL_RELEASED; }
	else                               { return CLUTCH_PEDAL_PRESSED;  }
}

////////////////////////////////////////////////////////////////////////////////////

bool gpio_engineRPM_getPinState(void) { return digitalRead(PIN_NEP); }

////////////////////////////////////////////////////////////////////////////////////

bool gpio_VSS_getPinState(void) { return digitalRead(PIN_VSS); }