#ifndef OLDNES_PPU_BUS_H
#define OLDNES_PPU_BUS_H

#include "definitions.h"
#include "mapper.h"

struct PPU;

typedef struct PPUBus {
    byte vram[0x800];
    byte palette[0x20];
    word nametable[4];
    struct Mapper* mapper;
} PPUBus;

void init_vram(struct PPU* ppu);

byte read_vram(struct PPUBus* ppu, word address);
void write_vram(struct PPUBus* ppu, word address, byte value);

#endif //OLDNES_PPU_BUS_H
