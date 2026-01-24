#include "muddersMIMA.h"

//CORDIC sqrt algorithm taken from https://www.convict.lu/Jeunes/Math/square_root_CORDIC.htm
uint16_t utility_sqrt_u16(uint16_t x)
{
    int base, i, y;
    base = 128;
    y = 0;
    for (i = 1; i <= 8; i++)
    {
        y += base;
        if ((y * y) > x)
        {
            y -= base; // base should not have been added, so we substract again
        }
        base >> 1; // shift 1 digit to the right = divide by 2
    }
    				Serial.println(y);
    return y;
}

//normalize a number from [MIN, MAX] to [0, 255]
uint8_t utility_normalizeRange_u8(uint8_t x, uint8_t min, uint8_t max)
{
    if(x > max) { return 255; }
    if(x < min) { return 0; }

    //               base at 0         //divide by total range 
    return (uint16_t)(x - min) * 255 / (max - min);
}

//scale a number from [0, 255] to [MIN, MAX]   (inverse of normalizeRange)
uint8_t utility_scaleRange_u8(uint8_t x, uint8_t min, uint8_t max)
{
    return min + ((uint16_t)x * (max - min)) / 255;
}

uint8_t utility_ease_out_circ_u8(uint8_t x)
{
    // x in range 0..255
    int16_t dx = 255 - x;          // (1 - x)
    uint16_t sq = dx * dx;         // (1 - x)^2  → 0..65025
    uint16_t y  = utility_sqrt_u16(65025 - sq);

    return (uint8_t)y;             // back to 0..255
}