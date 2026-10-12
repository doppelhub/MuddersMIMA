//Copyright 2022-2023(c) John Sullivan

#include "muddersMIMA.h"

////////////////////////////////////////////////////////////////////////////////////

volatile uint8_t mima_battery_soc = 50;
static volatile bool mima_soc_received = false;
volatile uint32_t spi_rx_count = 0;

////////////////////////////////////////////////////////////////////////////////////

void spiToLiBCM_begin(void)
{
    pinMode(MISO, OUTPUT);
    pinMode(MOSI, INPUT);
    pinMode(SCK, INPUT);
    pinMode(PIN_SPI_CS, INPUT_PULLUP);

    SPCR = _BV(SPE) | _BV(SPIE);

    SPSR &= ~_BV(SPI2X);

    uint8_t dummy = SPDR;
    (void)dummy;
}

////////////////////////////////////////////////////////////////////////////////////


void spiToLiBCM_handler(void)
{
    static uint32_t last_print = 0;

    if (millis() - last_print >= 1000)
    {
        last_print = millis();

        Serial.print(F("CS = "));
        Serial.print(digitalRead(PIN_SPI_CS));

        Serial.print(F("  SPI RX count = "));
        Serial.print(spi_rx_count);

        Serial.print(F("  SOC = "));
        Serial.println(mima_battery_soc);
    }
}

ISR(SPI_STC_vect)
{
    mima_battery_soc = SPDR;
    spi_rx_count++;
    mima_soc_received = true;
}

////////////////////////////////////////////////////////////////////////////////////
