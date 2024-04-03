#ifndef OLDNES_EMULATOR_H
#define OLDNES_EMULATOR_H

#include "cpu.h"
#include "memory.h"
#include "mapper.h"

typedef struct Emulator {
    struct CPU cpu;
    struct Memory memory;
    struct Mapper mapper;
    byte exit;
    byte pause;
} Emulator;

void init_emulator(struct Emulator* emulator, int argc, char* argv[]);
void free_emulator(struct Emulator* emulator);

void run_emulator(struct Emulator* emulator);

#endif //OLDNES_EMULATOR_H
