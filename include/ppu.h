#ifndef OLDNES_PPU_H
#define OLDNES_PPU_H

#include "definitions.h"
#include "ppu_bus.h"

#define SCANLINE_CYCLE_LENGTH 341
#define SCANLINE_CYCLE_END    340
#define SCANLINE_FRAME_END    261
#define SCANLINE_VISIBLE_DOTS 256
#define VISIBLE_SCANLINES     240

#define SCREEN_SIZE SCANLINE_VISIBLE_DOTS * VISIBLE_SCANLINES
#define OAM_SIZE 0x100
#define ATTRIBUTE_OFFSET 0x3C0

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
    word address;
} PPURegister;

struct Emulator;

typedef struct PPU {
    usize screen_buffer[SCREEN_SIZE];
    byte oam[OAM_SIZE];

    PPURegister vram;
    PPURegister temp;
    byte oam_address;
    byte first_write;
    byte data_buffer;
    byte fine_x;

    PPUCtrl ctrl;
    PPUMask mask;
    PPUStat stat;

    word scanline;
    word cycle;

    struct PPUBus*   bus;
    struct Emulator* emulator;
} PPU;

static const usize PALETTE[0x40] = {
        0xff545454, 0xff001e74, 0xff081090, 0xff300088, 0xff440064, 0xff5c0030, 0xff540400, 0xff3c1800,
        0xff202a00, 0xff083a00, 0xff004000, 0xff003c00, 0xff00323c, 0xff000000, 0xff000000, 0xff000000,
        0xff989698, 0xff084cc4, 0xff3032ec, 0xff5c1ee4, 0xff8814b0, 0xffa01464, 0xff982220, 0xff783c00,
        0xff545a00, 0xff287200, 0xff087c00, 0xff007628, 0xff006678, 0xff000000, 0xff000000, 0xff000000,
        0xffeceeec, 0xff4c9aec, 0xff787cec, 0xffb062ec, 0xffe454ec, 0xffec58b4, 0xffec6a64, 0xffd48820,
        0xffa0aa00, 0xff74c400, 0xff4cd020, 0xff38cc6c, 0xff38b4cc, 0xff3c3c3c, 0xff000000, 0xff000000,
        0xffeceeec, 0xffa8ccec, 0xffbcbcec, 0xffd4b2ec, 0xffecaeec, 0xffecaed4, 0xffecb4b0, 0xffe4c490,
        0xffccd278, 0xffb4de78, 0xffa8e290, 0xff98e2b4, 0xffa0d6e4, 0xffa0a2a0, 0xff000000, 0xff000000,
};

void init_ppu(struct Emulator* emulator);

void reset_ppu(struct PPU* ppu);
void execute_ppu(struct PPU* ppu);

void dma(struct PPU* ppu, byte address);

byte read_ppu(struct PPU* ppu);
byte read_status(struct PPU* ppu);
byte read_oam(struct PPU* ppu);

void write_ppu(struct PPU* ppu, byte value);
void set_control(struct PPU* ppu, byte ctrl);
void set_mask(struct PPU* ppu, byte mask);
void set_oam_address(struct PPU* ppu, byte address);
void set_vram_address(struct PPU* ppu, byte address);
void set_scroll(struct PPU* ppu, byte scroll);
void write_oam(struct PPU* ppu, byte value);

#endif //OLDNES_PPU_H
