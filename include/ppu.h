#ifndef OLDNES_PPU_H
#define OLDNES_PPU_H

#include "definitions.h"
#include "ppu_bus.h"

#define VISIBLE_SCANLINES 240
#define VISIBLE_DOTS 256
#define NTSC_SCANLINES_PER_FRAME 261
#define PAL_SCANLINES_PER_FRAME 311
#define DOTS_PER_SCANLINE 341
#define END_DOT 340

typedef union PPUCtrl {
    struct {
        byte nametable_x        : 1;
        byte nametable_y        : 1;
        byte increment_mode     : 1;
        byte pattern_sprite     : 1;
        byte pattern_background : 1;
        byte sprite_size        : 1;
        byte slave_mode         : 1;
        byte generate_nmi       : 1;
    };
    byte value;
} PPUCtrl;

typedef union PPUMask {
    struct {
        byte grey_scale      : 1;
        byte hide_background : 1;
        byte hide_sprites    : 1;
        byte show_background : 1;
        byte show_sprites    : 1;
        byte emphasize_red   : 1;
        byte emphasize_green : 1;
        byte emphasize_blue  : 1;
    };
    byte value;
} PPUMask;

typedef union PPUStat {
    struct {
        byte unused          : 5;
        byte sprite_overflow : 1;
        byte sprite_zero     : 1;
        byte vertical_blank  : 1;
    };
    byte value;
} PPUStat;

typedef union LoopyRegister {
    struct {
        word coarse_x    : 5;
        word coarse_y    : 5;
        word nametable_x : 1;
        word nametable_y : 1;
        word fine_y      : 3;
        word unused      : 1;
    };
    word value;
} PPURegister;

struct Emulator;

typedef struct PPU {
    usize screen_buffer[VISIBLE_DOTS * VISIBLE_SCANLINES];
    byte oam[0x100];

    PPUCtrl ctrl;
    PPUMask mask;
    PPUStat stat;

    struct PPUBus*   bus;
    struct Emulator* emulator;

} PPU;

void init_ppu(struct Emulator* emulator);

void reset_ppu(struct PPU* ppu);
void execute_ppu(struct PPU* ppu);

void dma(struct PPU* ppu, byte address);

byte read_ppu(struct PPU* ppu);
void write_ppu(struct PPU* ppu, byte value);

byte read_status(struct PPU* ppu);
byte read_oam_data(struct PPU* ppu);

void set_control(struct PPU* ppu, byte ctrl);
void set_mask(struct PPU* ppu, byte mask);
void set_oam_address(struct PPU* ppu, byte address);
void set_data_address(struct PPU* ppu, byte address);
void set_scroll(struct PPU* ppu, byte scroll);
void set_data(struct PPU* ppu, byte data);
void set_oam_data(struct PPU* ppu, byte value);

byte read_oam(struct PPU* ppu, byte address);
void write_oam(struct PPU* ppu, byte address, byte value);

#endif //OLDNES_PPU_H
