#ifndef OLDNES_EMULATOR_H
#define OLDNES_EMULATOR_H

#include "cpu.h"
#include "ppu.h"
#include "graphics.h"
#include "cpu_bus.h"
#include "mapper.h"

#define NES_VIDEO_WIDTH  256
#define NES_VIDEO_HEIGHT 240

typedef struct Emulator {
    struct CPU cpu;
    struct PPU ppu;
    struct CPUBus cpu_bus;
    struct PPUBus ppu_bus;
    struct Mapper mapper;
    struct GraphicsContext gfx;
    byte exit;
    byte pause;
} Emulator;

void init_emulator(struct Emulator* emulator, int argc, char* argv[]);
void free_emulator(struct Emulator* emulator);

void run_emulator(struct Emulator* emulator);

#endif //OLDNES_EMULATOR_H
