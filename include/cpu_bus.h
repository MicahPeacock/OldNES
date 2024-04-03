#ifndef OLDNES_CPU_BUS_H
#define OLDNES_CPU_BUS_H

#include "definitions.h"
#include "controller.h"
#include "mapper.h"


#define RAM_SIZE 0x0800

typedef enum IORegisters {
    PPUCTRL = 0x2000,
    PPUMASK = 0x2001,
    PPUSTAT = 0x2002,
    OAMADDR = 0x2003,
    OAMDATA = 0x2004,
    PPUSCRL = 0x2005,
    PPUADDR = 0x2006,
    PPUDATA = 0x2007,
    OAMDMA  = 0x4014,
    JOYPAD1 = 0x4016,
    JOYPAD2 = 0x4017,
} IORegisters;

struct Emulator;

typedef struct CPUBus {
    byte ram[RAM_SIZE];
    struct Controller pad1;
    struct Controller pad2;

    struct Mapper*   mapper;
    struct Emulator* emulator;
} CPUBus;

void init_memory(struct Emulator* emulator);

byte read_memory(struct CPUBus* bus, word address);
void write_memory(struct CPUBus* bus, word address, byte value);
byte* get_page_ptr(struct CPUBus* bus, word address);

#endif //OLDNES_CPU_BUS_H
