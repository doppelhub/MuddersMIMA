//Copyright 2022-2024(c) John Sullivan

#ifndef spiToLiBCM_h
#define spiToLiBCM_h

//==============================================================================
// Public Interface Functions
//==============================================================================

void spiToLiBCM_begin(void);
void spiToLiBCM_handler(void);
bool spiToLiBCM_isConnected(void);

//==============================================================================
// LiBCM Battery Data Access Functions
//==============================================================================

uint8_t spiToLiBCM_getLiBCM_SoC_percent(void);
uint8_t spiToLiBCM_getLiBCM_Temperature_celsius(void);
uint8_t spiToLiBCM_getLiBCM_ErrorFlags(void);
uint8_t spiToLiBCM_getLiBCM_PackVoltage_scaled(void);
int16_t spiToLiBCM_getLiBCM_Power_watts(void);
int16_t spiToLiBCM_getLiBCM_Current_deciAmps(void);

//==============================================================================
// Interface Functions (implemented in spiToLiBCM.cpp)
//==============================================================================

uint8_t spiToLiBCM_getCurrentMode(void);
uint8_t spiToLiBCM_getAssistLevel(void);
uint8_t spiToLiBCM_getRegenLevel(void);
uint8_t spiToLiBCM_getErrorFlags(void);
int16_t spiToLiBCM_getInstantaneousPower(void);
int16_t spiToLiBCM_getCurrent_deciAmps(void);
void spiToLiBCM_emergencyStop(void);

//==============================================================================
// MIMA Mode Constants (Toggle Switch Positions)
//==============================================================================

#define MIMA_TOGGLE_POSITION0  0    // Typically OEM mode
#define MIMA_TOGGLE_POSITION1  1    // Typically manual mode variant 1
#define MIMA_TOGGLE_POSITION2  2    // Typically manual mode variant 2
#define MIMA_TOGGLE_UNDEFINED  3    // Error/undefined state

//==============================================================================
// Error Flag Constants
//==============================================================================

#define MIMA_ERROR_NONE           0x00
#define MIMA_ERROR_OVERHEAT       0x01
#define MIMA_ERROR_OVERCURRENT    0x02
#define MIMA_ERROR_COMMUNICATION  0x04
#define MIMA_ERROR_SENSOR         0x08
#define MIMA_ERROR_SAFETY         0x10

#endif