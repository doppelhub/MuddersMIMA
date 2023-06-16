//Copyright 2022-2023(c) John Sullivan


//stores various system millisecond timers

#include "muddersMIMA.h"

uint8_t loopPeriod_ms = 10;

uint32_t lastTimeMAMODE1_wasInvalid = 0;

////////////////////////////////////////////////////////////////////////////////////

void time_loopPeriod_ms_set(uint8_t period_ms) { loopPeriod_ms = period_ms; }
uint8_t time_loopPeriod_ms_get(void) { return loopPeriod_ms; }

////////////////////////////////////////////////////////////////////////////////////

void time_waitForLoopPeriod(void)
{
    static uint32_t timestamp_previousLoopStart_ms = millis();

    while( (millis() - timestamp_previousLoopStart_ms ) < time_loopPeriod_ms_get() ) { ; } //wait here to start next loop

    timestamp_previousLoopStart_ms = millis(); //placed at end to prevent delay at keyON event
}

////////////////////////////////////////////////////////////////////////////////////

//calculate delta between start and stop time
//store start time: START_TIMER
//calculate delta:  STOP_TIMER
void time_stopwatch(bool timerAction)
{
    static uint32_t startTime = 0;

    if(timerAction == START_TIMER) { startTime = millis(); }
    else
    {
        uint32_t stopTime = millis();
        Serial.print(F("\nDelta: "));
        Serial.print(stopTime - startTime);
        Serial.print(F(" ms\n"));
    }
}

////////////////////////////////////////////////////////////////////////////////////

//only works from 1 to 255 Hz
uint16_t time_hertz_to_milliseconds(uint8_t hertz)
{
    uint16_t milliseconds = 0;

    if( (hertz != 0) ) { milliseconds = 1000 / hertz; } //division

    return milliseconds;
}

////////////////////////////////////////////////////////////////////////////////////

uint32_t time_latestKeyOn_ms(void) { return lastTimeMAMODE1_wasInvalid; }

////////////////////////////////////////////////////////////////////////////////////

void time_handler(void)
{
    //LiControl can't directly read when the key turns on.
    //If the USB cable is unplugged, then LiControl also powers on at each keyON event.  In this case millis() ~= latestKeyON...
    //However, if a laptop is plugged in when the key is off, then LiControl is already running when the key turns on. In this case millis() != latestKeyON.
    //Therefore, we can't use "millis()" to determine when the latest keyON event occurred.
    //Instead, look at when the ECM first starts sending valid data, which only happens when the key is on.
    //Specifically, continuously store the last time the data WASN'T valid, which is immediately before the key turned on.
    if( (ecm_getMAMODE1_state() == MAMODE1_STATE_IS_ERROR_LO ) ||
        (ecm_getMAMODE1_state() == MAMODE1_STATE_IS_ERROR_HI ) ||
        (ecm_getMAMODE1_state() == MAMODE1_STATE_IS_UNDEFINED)  ) { lastTimeMAMODE1_wasInvalid = millis(); } //ECM is unpowered (i.e. key is OFF)
}