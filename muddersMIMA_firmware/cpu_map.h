//Copyright 2022(c) John Sullivan


//cpu_map.h - CPU and pin mapping configuration file
//central pin mapping selection file for different processor types

#ifndef cpu_map_h
#define cpu_map_h

  #ifdef CPU_MAP_ATMEGA328p
  
      #define PIN_USER_JOYSTICK  A0
      #define PIN_MAMODE1_ECM    A1
      #define PIN_CMDPWR_ECM     A2
      #define PIN_USER_MOMENTARY A3
      #define PIN_USER_TOGGLE1   A4
      #define PIN_USER_TOGGLE2   A5

      // #define PIN_METSCI_DE    2
      // #define PIN_METSCI_REn   3
      // #define PIN_TURNOFFLiBCM 4
      // #define PIN_VPIN_OUT_PWM 5
      // #define PIN_SENSOR_EN    6
      // #define PIN_MCME_PWM     7
      #define PIN_MAMODE2_ECM     8
      #define PIN_BUZZER          9
      #define PIN_CMDPWR_MCM     10
      #define PIN_MAMODE1_MCM    11
      #define PIN_MAMODE2_MCM    12
      #define PIN_LED_UNO        13

  #endif

#endif