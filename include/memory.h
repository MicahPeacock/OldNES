#ifndef OLDNES_MEMORY_H
#define OLDNES_MEMORY_H

#include "definitions.h"

#define RAM_SIZE 0x10000

typedef struct Memory {
    byte ram[RAM_SIZE];
} Memory;

void load_memory(const char* filename, Memory* memory);

byte read_memory(const Memory* memory, word address);
void write_memory(Memory* memory, word address, byte value);

#endif //OLDNES_MEMORY_H
