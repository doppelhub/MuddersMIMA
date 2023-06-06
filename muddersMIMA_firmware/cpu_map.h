//Copyright 2022(c) John Sullivan


//cpu_map.h - CPU and pin mapping configuration file
//central pin mapping selection file for different processor types

#ifndef cpu_map_h
#define cpu_map_h

  #ifdef CPU_MAP_ATMEGA328p
      #ifdef HW_PROTO
            #define PIN_USER_JOYSTICK  A0
            #define PIN_MAMODE1_ECM    A1
            #define PIN_CMDPWR_ECM     A2
            #define PIN_USER_MOMENTARY A3
            #define PIN_USER_TOGGLE1   A4
            #define PIN_USER_TOGGLE2   A5

            #define PIN_MAMODE2_ECM     8
            #define PIN_CMDPWR_MCM     10
            #define PIN_MAMODE1_MCM    11
            #define PIN_MAMODE2_MCM    12
            #define PIN_SPI_SCK_LED    13

      #elif defined HW_REVA_REVB
            #define PIN_USER_JOYSTICK  A0
            #define PIN_MAMODE1_ECM    A1
            #define PIN_CMDPWR_ECM     A2
            #define PIN_USER_MOMENTARY A3
            #define PIN_USER_TOGGLE1   A4
            #define PIN_USER_TOGGLE2   A5
            #define PIN_MAP_SENSOR     A6
            #define PIN_THROTTLE       A7

            #define PIN_MAMODE2_MCM     2
            #define PIN_MAMODE1_MCM     3
            #define PIN_MAMODE2_ECM     4
            #define PIN_BRAKE           5
            #define PIN_CLUTCH          6
            #define PIN_VSS             7
            #define PIN_NEP             8
            #define PIN_CMDPWR_MCM      9
            #define PIN_SPI_CS         10
            #define PIN_SPI_MOSI       11
            #define PIN_SPI_MISO       12
            #define PIN_SPI_SCK_LED    13

      #endif

  #endif

#endif