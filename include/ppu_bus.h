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

byte ppu_read(struct PPUBus* bus, word address);
void ppu_write(struct PPUBus* bus, word address, byte value);

#endif //OLDNES_PPU_BUS_H
