#ifndef OLDNES_CPU_H
#define OLDNES_CPU_H

#include "memory.h"

#define NMI_VECTOR   0xfffa
#define RESET_VECTOR 0xfffc
#define IRQ_VECTOR   0xfffe

#define STACK_BASE  0x0100
#define STACK_RESET 0xfd

typedef union StatusFlags {
    struct {
        byte c : 1; // Carry flag
        byte z : 1; // Zero flag
        byte i : 1; // Interrupt Disable flag
        byte d : 1; // Decimal flag
        byte b : 1; // Break flag
        byte u : 1; // Unused Constant (1)
        byte v : 1; // Overflow flag
        byte n : 1; // Negative flag
    };
    byte value;
} StatusFlags;

typedef enum StatusBits {
    NEGATIVE_BIT  = 1 << 7,
    OVERFLOW_BIT  = 1 << 6,
    UNUSED_BIT    = 1 << 5,
    BREAK_BIT     = 1 << 4,
    DECIMAL_BIT   = 1 << 3,
    INTERRUPT_BIT = 1 << 2,
    ZERO_BIT      = 1 << 1,
    CARRY_BIT     = 1,
} StatusBits;

typedef struct CPU {
    word pc;
    byte sp;
    byte a, x, y;
    StatusFlags status;
} CPU;

void reset_cpu(CPU* cpu);
byte execute(CPU* cpu, Memory* memory);

#endif //OLDNES_CPU_H
