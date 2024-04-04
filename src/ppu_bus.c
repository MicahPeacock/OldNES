#include <stdlib.h>
#include <string.h>

#include "ppu_bus.h"
#include "emulator.h"

static void set_mirroring(struct PPUBus* bus);
static void set_mirror_mapping(struct PPUBus* bus, word tr, word tl, word br, word bl);

static word to_palette_address(word address);
static word to_vram_address(word address);

void init_ppu_bus(struct Emulator* emulator) {
    struct PPUBus* bus = &emulator->ppu_bus;
    bus->mapper = &emulator->mapper;

    set_mirroring(bus);
    memset(bus->vram,    0, 0x800);
    memset(bus->palette, 0,  0x20);
}

byte ppu_read(struct PPUBus* bus, word address) {
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

void ppu_write(struct PPUBus* bus, word address, byte value) {
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
    switch (bus->mapper->mirroring) {
        case VERTICAL:
            set_mirror_mapping(bus, 0x000, 0x400, 0x000, 0x400);
            break;
        case HORIZONTAL:
            set_mirror_mapping(bus, 0x000, 0x000, 0x400, 0x400);
            break;
        case ONE_SCREEN:
        case ONE_SCREEN_LOWER:
            set_mirror_mapping(bus, 0x000, 0x000, 0x000, 0x000);
            break;
        case ONE_SCREEN_UPPER:
            set_mirror_mapping(bus, 0x400, 0x400, 0x400, 0x400);
            break;
        case FOUR_SCREEN:
            set_mirror_mapping(bus, 0x000, 0x000, 0x000, 0x000);
            break;
        default:
            set_mirror_mapping(bus, 0x000, 0x000, 0x000, 0x000);
            break;
    }
}

static void set_mirror_mapping(struct PPUBus* bus, word tr, word tl, word br, word bl) {
    bus->nametable[0] = tr;
    bus->nametable[1] = tl;
    bus->nametable[2] = br;
    bus->nametable[3] = bl;
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