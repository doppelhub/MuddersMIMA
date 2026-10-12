#ifndef spiToLiBCM_h
#define spiToLiBCM_h

#include <stdint.h>

void spiToLiBCM_begin(void);
void spiToLiBCM_handler(void);

extern volatile uint8_t mima_battery_soc;

#endif
