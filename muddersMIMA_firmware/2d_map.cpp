//Copyright 2024(c) Martin Kennedy

#include "muddersMIMA.h"

//Evaluate functions expressed as a 2D arrays

/* Example map:

   .  0  1  2
   0  2  4  8
   1  3  6 10
   2  5 10 15

   0.1 x 0.4 -> ([0, 0] * 0.9 * 0.6 + [0, 1] * 0.1 * 0.6 + [1, 0] * 0.9 * 0.4 + [1, 1] * 0.1 * 0.4)
             -> (     2 * 0.9 * 0.6 +      4 * 0.1 * 0.6 +      3 * 0.9 * 0.4 +      6 * 0.1 * 0.4)
             -> 2.64
 */
uint8_t evaluate_2d_map(uint8_t* map, uint8_t* dim1_map, uint8_t dim1_size, uint8_t dim1, uint8_t* dim2_map, uint8_t dim2_size, uint8_t dim2) {
    uint8_t dim1_res = 0;
    uint8_t dim2_res = 0;
    uint16_t dim1_rem = 0;
    uint16_t dim2_rem = 0;

    while (dim1_res < dim1_size) {
        if (dim1 < dim1_map[dim1_res]) {
            if (dim1_res == 0) {
                dim1_rem = 0;
            } else {
                dim1_res--;
                dim1_rem = (256 * (dim1 - dim1_map[dim1_res])) / (dim1_map[dim1_res+1] - dim1_map[dim1_res]);
            }
            break;
        }
        dim1_res++;
    }

    if (dim1_res == dim1_size) {
        dim1_rem = 256;
        dim1_res = dim1_res - 2;
    }

    while (dim2_res < dim2_size) {
        if (dim2 < dim2_map[dim2_res]) {
            if (dim2_res == 0) {
                dim2_rem = 0;
            } else {
                dim2_res--;
                dim2_rem = (256 * (dim2 - dim2_map[dim2_res])) / (dim2_map[dim2_res+1] - dim2_map[dim2_res]);
            }
            break;
        }
        dim2_res++;
    }

    if (dim2_res == dim2_size) {
        dim2_rem = 256;
        dim1_res = dim1_res - 2;
    }

    uint8_t image = \
        ((map[dim1_size * dim2_res + dim1_res                   ] * (256 - dim1_rem)) / 256 * (256 - dim2_rem)) / 256 \
        + ((map[dim1_size * dim2_res + dim1_res + 1             ] * (dim1_rem)      ) / 256 * (256 - dim2_rem)) / 256 \
        + ((map[dim1_size * dim2_res + dim1_res + dim1_size     ] * (256 - dim1_rem)) / 256 * (      dim2_rem)) / 256 \
        + ((map[dim1_size * dim2_res + dim1_res + dim1_size + 1 ] * (dim1_rem)      ) / 256 * (      dim2_rem)) / 256;

    return image;
}