#ifndef OLDNES_DEFINITIONS_H
#define OLDNES_DEFINITIONS_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t  byte;
typedef uint16_t word;
typedef uint32_t usize;

typedef int8_t   sbyte;
typedef int16_t  sword;
typedef int32_t  ssize;

typedef enum Bits {
    BIT_7 = 1 << 7,
    BIT_6 = 1 << 6,
    BIT_5 = 1 << 5,
    BIT_4 = 1 << 4,
    BIT_3 = 1 << 3,
    BIT_2 = 1 << 2,
    BIT_1 = 1 << 1,
    BIT_0 = 1,
} Bits;

#endif //OLDNES_DEFINITIONS_H
