#ifndef OLDNES_PPU_BUS_H
#define OLDNES_PPU_BUS_H

#include "definitions.h"
#include "mapper.h"

struct Emulator;

typedef struct PPUBus {
    byte vram[0x800];
    byte palette[0x20];
    word nametable[4];
    struct Mapper* mapper;
} PPUBus;

void init_ppu_bus(struct Emulator* emulator);

byte read_ppu_memory(const struct PPUBus* bus, word address);
void write_ppu_memory(struct PPUBus* bus, word address, byte value);

#endif //OLDNES_PPU_BUS_H
