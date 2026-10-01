#ifndef utility_h
    #define utility_h

    uint16_t utility_sqrt_u16(uint16_t x);

    uint8_t utility_normalizeRange_u8(uint8_t x, uint8_t min, uint8_t max);

    uint8_t utility_scaleRange_u8(uint8_t x, uint8_t min, uint8_t max);

    uint8_t utility_ease_out_circ_u8(uint8_t x);

    uint8_t utility_ease_out_cubic_u8(uint8_t t);

#endif