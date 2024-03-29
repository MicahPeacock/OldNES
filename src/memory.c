#include "memory.h"

void load_memory(const char* filename, Memory* memory) {

}

byte read_memory(const Memory* memory, word address) {
    return memory->ram[address];
}

void write_memory(Memory* memory, word address, byte value) {
    memory->ram[address] = value;
}