#include <stdlib.h>
#include <string.h>

#include "ppu_bus.h"
#include "ppu.h"

static void set_mirroring(struct PPUBus* bus);

static word to_palette_address(word address);
static word to_vram_address(word address);

void init_vram(struct PPU* ppu) {
    struct PPUBus* bus = ppu->bus;

    memset(bus->vram,    0, 0x800);
    memset(bus->palette, 0,  0x20);
}

byte read_vram(struct PPUBus* bus, word address) {
    if (address < 0x2000) {
        return bus->mapper->read_chr(bus->mapper, address);
    }
    if (address < 0x3f00) {
        return bus->vram[bus->nametable[address / 0x400] + to_vram_address(address)];
    }
    if (address < 0x4000) {
        return bus->palette[to_palette_address(address)];
    }
    return 0;
}

void write_vram(struct PPUBus* bus, word address, byte value) {
    if (address < 0x2000) {
        bus->mapper->write_chr(bus->mapper, address, value);
        return;
    }
    if (address < 0x3f00) {
        bus->vram[bus->nametable[address / 0x400] + to_vram_address(address)] = value;
        return;
    }
    if (address < 0x4000) {
        bus->palette[to_palette_address(address)] = value;
    }
}

static void set_mirroring(struct PPUBus* bus) {

}

static word to_palette_address(word address) {
    const byte palette_addr = address & 0x1f;
    if (palette_addr >= 0x10 && (address & 0x04) == 0) {
        return palette_addr & 0x0f;
    }
    return palette_addr;
}

static word to_vram_address(word address) {

}