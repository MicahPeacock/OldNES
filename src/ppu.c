#include "ppu.h"
#include "emulator.h"

static byte rendering_enabled(const struct PPU* ppu);
static byte get_vram_increment(const struct PPU* ppu);

static void increment_scroll_x(struct PPU* ppu);
static void increment_scroll_y(struct PPU* ppu);
static void transfer_address_x(struct PPU* ppu);
static void transfer_address_y(struct PPU* ppu);

static word render_background(struct PPU* ppu);
static word render_sprites(struct PPU* ppu, word background_address);

void init_ppu(struct Emulator* emulator) {
    struct PPU* ppu = &emulator->ppu;
    ppu->emulator = emulator;
    ppu->bus = &emulator->ppu_bus;

    memset(ppu->oam, 0, OAM_SIZE);
    ppu->vram.address = 0x0000;
    ppu->oam_address = 0;
    reset_ppu(ppu);
}

void reset_ppu(struct PPU* ppu) {
    ppu->temp.address = 0x0000;
    ppu->ctrl.value  = 0x00;
    ppu->mask.value  = 0x00;
    ppu->stat.value  = 0x00;
    ppu->data_buffer = 0x00;
    ppu->fine_x      = 0x00;
    ppu->scanline = 0;
    ppu->cycle    = 0;
    ppu->first_write = 1;
    ppu->mask.show_background = 1;
    ppu->mask.show_sprites    = 1;
    memset(ppu->screen_buffer, 0, sizeof(ppu->screen_buffer));
}

void execute_ppu(struct PPU* ppu) {

}

void dma(struct PPU* ppu, byte address) {
    const struct CPUBus* bus = &ppu->emulator->cpu_bus;
    const byte* ptr = get_page_ptr(bus, address);
    memcpy(ppu->oam + ppu->oam_address, ptr, 256 - ppu->oam_address);
    if (ppu->oam_address) {
        memcpy(ppu->oam, ptr + (256 - ppu->oam_address), ppu->oam_address);
    }
    ppu->emulator->cpu.skip_cycles += 513;
    ppu->emulator->cpu.skip_cycles += ppu->emulator->cpu.cycles & 1;
}

byte read_ppu(struct PPU* ppu) {
    byte data = ppu->data_buffer;
    ppu->data_buffer = ppu_read(ppu->bus, ppu->vram.address);
    if (ppu->vram.address >= 0x3f00) {
        data = ppu->data_buffer;
    }
    ppu->vram.address += get_vram_increment(ppu);
    return data;
}

byte read_status(struct PPU* ppu) {
    const byte status = ppu->stat.value;
    ppu->stat.vertical_blank = 0;
    ppu->first_write = 1;
    return status;
}

byte read_oam(struct PPU* ppu) {
    return ppu->oam[ppu->oam_address];
}

void write_ppu(struct PPU* ppu, byte value) {
    ppu_write(ppu->bus, ppu->vram.address, value);
    ppu->vram.address += get_vram_increment(ppu);
}

void set_control(struct PPU* ppu, byte ctrl) {
    ppu->ctrl.value = ctrl;
    ppu->temp.nametable_x = ppu->ctrl.nametable_x;
    ppu->temp.nametable_y = ppu->ctrl.nametable_y;
}

void set_mask(struct PPU* ppu, byte mask) {
    ppu->mask.value = mask;
}

void set_oam_address(struct PPU* ppu, byte address) {
    ppu->oam_address = address;
}

void set_vram_address(struct PPU* ppu, byte address) {
    if (ppu->first_write) {
        ppu->temp.address = (ppu->temp.address & 0xff00) | address;
        ppu->vram = ppu->temp;
        ppu->first_write = 0;
    } else {
        ppu->temp.address = (word)((address & 0x3f) << 8) | (ppu->temp.address & 0x00ff);
        ppu->first_write = 1;
    }
}

void set_scroll(struct PPU* ppu, byte scroll) {
    if (ppu->first_write) {
        ppu->temp.coarse_x = scroll >> 3;
        ppu->fine_x        = scroll & 0x7;
        ppu->first_write = 0;
    } else {
        ppu->temp.coarse_y = scroll >> 3;
        ppu->temp.fine_y   = scroll & 0x7;
        ppu->first_write = 1;
    }
}

void write_oam(struct PPU* ppu, byte value) {
    ppu->oam[ppu->oam_address++] = value;
}

static byte rendering_enabled(const struct PPU* ppu) {
    return ppu->mask.show_background || ppu->mask.show_sprites;
}

static byte get_vram_increment(const struct PPU* ppu) {
    return ppu->ctrl.increment_mode ? 0x20 : 0x01;
}

static void increment_scroll_x(struct PPU* ppu) {
    if (!rendering_enabled(ppu)) return;
    if (ppu->vram.coarse_x == 0x1f) {
        ppu->vram.coarse_x = 0;
        ppu->vram.nametable_x = ~ppu->vram.nametable_x;
    } else {
        ppu->vram.coarse_x++;
    }
}

static void increment_scroll_y(struct PPU* ppu) {
    if (!rendering_enabled(ppu)) return;
    if (ppu->vram.fine_y < 0x07) {
        ppu->vram.fine_y++;
    } else {
        ppu->vram.fine_y = 0;
        if (ppu->vram.coarse_y == 0x1d) {
            ppu->vram.coarse_y = 0;
            ppu->vram.nametable_y = ~ppu->vram.nametable_y;
        } else if (ppu->vram.coarse_y == 0x1f) {
            ppu->vram.coarse_y = 0;
        } else {
            ppu->vram.coarse_y++;
        }
    }
}

static void transfer_address_x(struct PPU* ppu) {
    if (!rendering_enabled(ppu)) return;
    ppu->vram.nametable_x = ppu->temp.nametable_x;
    ppu->vram.coarse_x    = ppu->temp.coarse_x;
}

static void transfer_address_y(struct PPU* ppu) {
    ppu->vram.nametable_y = ppu->temp.nametable_y;
    ppu->vram.coarse_y    = ppu->temp.coarse_y;
    ppu->vram.fine_y      = ppu->temp.fine_y;
}

static word render_background(struct PPU* ppu) {

}

static word render_sprites(struct PPU* ppu, word background_address) {

}