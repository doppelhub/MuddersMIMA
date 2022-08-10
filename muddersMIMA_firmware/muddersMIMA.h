//Copyright 2022(c) John Sullivan


//main LiBCM include file

#ifndef limima_h
  #define limima_h

  #define NO  false
  #define YES true

  #define OFF false
  #define ON  true

  //define standard libraries used by LiBCM
  #include <Arduino.h>
  #include <avr/wdt.h>

  //Define LiBCM system include files.  Note: Do not alter order.
  #include "config.h"
  #include "cpu_map.h"
  #include "debugLED.h"
  #include "debugUSB.h"
  #include "gpio.h"
  #include "adc.h"
  #include "time.h"
  #include "USB_userInterface.h"
  #include "ecm_signals.h"

#endif