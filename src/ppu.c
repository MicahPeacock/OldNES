#include "ppu.h"
#include "emulator.h"

void init_ppu(struct Emulator* emulator) {
    struct PPU* ppu = &emulator->ppu;
    ppu->emulator = emulator;
    ppu->bus = &emulator->ppu_bus;
    init_vram(ppu);
    reset_ppu(ppu);
}

void reset_ppu(struct PPU* ppu) {

}

void execute_ppu(struct PPU* ppu) {

}

byte read_ppu(struct PPU* ppu) {
    return 0;
}

void write_ppu(struct PPU* ppu, byte value) {

}

void dma(struct PPU* ppu, byte address) {
//    struct CPUBus* bus = &ppu->emulator->bus;
//    const byte* ptr = get_page_ptr(bus, address);
//    memcpy(ppu->oam + ppu->oam_address, ptr, 256 - ppu->oam_address);
//    if (ppu->oam_address) {
//        memcpy(ppu->oam, ptr + (256 - ppu->oam_address), ppu->oam_address);
//    }
    ppu->emulator->cpu.skip_cycles += 513;
    ppu->emulator->cpu.skip_cycles += ppu->emulator->cpu.cycles & 1;
}

byte read_status(struct PPU* ppu) {
    return 0;
}

byte read_oam_data(struct PPU* ppu) {
    return 0;
}

void set_control(struct PPU* ppu, byte ctrl) {

}

void set_mask(struct PPU* ppu, byte mask) {

}

void set_oam_address(struct PPU* ppu, byte address) {

}

void set_data_address(struct PPU* ppu, byte address) {

}

void set_scroll(struct PPU* ppu, byte scroll) {

}

void set_oam_data(struct PPU* ppu, byte value) {

}