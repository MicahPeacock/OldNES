#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "emulator.h"

void init_memory(struct Emulator* emulator) {
    struct Memory* memory = &emulator->memory;
    memory->emulator = emulator;
    memory->mapper   = &emulator->mapper;
    memset(memory->ram, 0, RAM_SIZE);
}

byte read_memory(const Memory* memory, word address) {
    if (address < 0x2000) {
        return memory->ram[address];
    } else {
        return memory->mapper->read_prg(memory->mapper, address);
    }
}

void write_memory(Memory* memory, word address, byte value) {
    if (address < 0x2000) {
        memory->ram[address & 0x7ff] = value;
        return;
    }
}

byte* get_page_ptr(Memory* memory, word address) {
    if (address < 0x2000)
        return &memory->ram[address & 0x7ff];
    exit(EXIT_FAILURE);
}