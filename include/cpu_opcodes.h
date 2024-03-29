#ifndef OLDNES_CPU_OPCODES_H
#define OLDNES_CPU_OPCODES_H

#include "definitions.h"

typedef enum AddressMode {
    IMM,
    ZPG, ZPX, ZPY,
    ABS, ABX, ABY,
    IND, IDX, IDY,
    REL, ACC, IMP,
} AddressMode;

typedef enum Operation {
    ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC, CLD, CLI, CLV, CMP, CPX,
    CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP, JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA,
    PLP, ROL, ROR, RTI, RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXS, TXA, TYA,

    XXX, // Unused Opcode
} Operation;

typedef struct Instruction {
    byte opcode;
    Operation operation;
    AddressMode address_mode;
    byte cycles;
} Instruction;

extern const Instruction INSTRUCTIONS[0x100];

#endif //OLDNES_CPU_OPCODES_H
