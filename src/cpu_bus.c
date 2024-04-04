#include <stdlib.h>
#include <string.h>

#include "cpu_bus.h"
#include "emulator.h"
#include "log.h"

void init_cpu_bus(struct Emulator* emulator) {
    struct CPUBus* bus = &emulator->cpu_bus;
    bus->emulator = emulator;
    bus->mapper = &emulator->mapper;

    memset(bus->ram, 0, RAM_SIZE);
    init_controller(&bus->pad1, 0);
    init_controller(&bus->pad2, 1);
}

byte cpu_read(struct CPUBus* bus, word address) {
    if (address < 0x2000) {
        return bus->ram[address];
    }
    if (address < 0x4000) {
        address &= 0x2007;
    }
    if (address < 0x4020) {
        struct PPU* ppu = &bus->emulator->ppu;
        switch (address) {
            case PPUSTAT:
                return read_status(ppu);
            case OAMDATA:
                return read_oam(ppu);
            case PPUDATA:
                return read_ppu(ppu);
            case JOYPAD1:
                return read_controller(&bus->pad1);
            case JOYPAD2:
                return read_controller(&bus->pad2);
            default:
                LOG(DEBUG, "Cannot read from register 0x%x", address);
                return 0;
        }
    }
    if (address < 0x6000) {
        LOG(DEBUG, "Attempted to read from expansion ROM");
        return 0;
    }
    if (address < 0x8000) {
        LOG(DEBUG, "Extended RAM not supported yet");
        return 0;
    }
    else {
        return bus->mapper->read_prg(bus->mapper, address);
    }
}

void cpu_write(struct CPUBus* bus, word address, byte value) {
    if (address < 0x2000) {
        bus->ram[address & 0x7ff] = value;
        return;
    }
    if (address < 0x4000) {
        address &= 0x2007;
    }
    if (address < 0x4020) {
        struct PPU* ppu = &bus->emulator->ppu;
        switch (address) {
            case PPUCTRL:
                set_control(ppu, value);
                return;
            case PPUMASK:
                set_mask(ppu, value);
                return;
            case PPUSCRL:
                set_scroll(ppu, value);
                return;
            case PPUADDR:
                set_vram_address(ppu, value);
                return;
            case PPUDATA:
                write_ppu(ppu, value);
                return;
            case OAMADDR:
                set_oam_address(ppu, value);
                return;
            case OAMDMA:
                dma(ppu, value);
                return;
            case OAMDATA:
                write_oam(ppu, value);
                return;
            case JOYPAD1:
                write_controller(&bus->pad1, value);
                write_controller(&bus->pad2, value);
                return;
            default:
                LOG(DEBUG, "Cannot write to register 0x%x", address);
                return;
        }
    }
    if (address < 0x6000) {
        LOG(DEBUG, "Attempted to write to expansion ROM");
        return;
    }
    if (address < 0x8000) {
        LOG(DEBUG, "Extended RAM not supported yet");
        return;
    }
    else {
        bus->mapper->write_prg(bus->mapper, address, value);
    }
}

const byte* get_page_ptr(const struct CPUBus* bus, word address) {
    if (address < 0x2000) {
        return &bus->ram[address & 0x7ff];
    }
    LOG(ERROR, "Could not access pointer to address 0x%x", address);
    exit(EXIT_FAILURE);
}