#ifndef OLDNES_MEMORY_H
#define OLDNES_MEMORY_H

#include "definitions.h"
#include "mapper.h"

#define RAM_SIZE 0x0800

struct Emulator;

typedef struct Memory {
    byte ram[RAM_SIZE];
    struct Emulator* emulator;
    struct Mapper* mapper;
} Memory;

void init_memory(struct Emulator* emulator);

byte read_memory(const Memory* memory, word address);
void write_memory(Memory* memory, word address, byte value);
byte* get_page_ptr(Memory* memory, word address);

#endif //OLDNES_MEMORY_H
