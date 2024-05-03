#ifndef OLDNES_MAPPER_H
#define OLDNES_MAPPER_H

#include "definitions.h"

typedef enum Mirroring {
    VERTICAL,
    HORIZONTAL,
    ONE_SCREEN,
    ONE_SCREEN_LOWER,
    ONE_SCREEN_UPPER,
    FOUR_SCREEN,
} Mirroring;

typedef enum MapperID {
    NROM  = 0,
    MMC1  = 1,
    UXROM = 2,
    CNROM = 3,
    MMC3  = 4,
    AOROM = 7,
    GNROM = 66,
} MapperID;

typedef struct Mapper {
    byte* prg_rom;
    byte* chr_rom;
    byte  prg_banks;
    byte  chr_banks;
    usize clamp;

    MapperID  mapper_id;
    Mirroring mirroring;

    byte (*read_prg)(const struct Mapper* mapper, word address);
    byte (*read_chr)(const struct Mapper* mapper, word address);
    void (*write_prg)(struct Mapper* mapper, word address, byte value);
    void (*write_chr)(struct Mapper* mapper, word address, byte value);

    void (*scanline_irq)(struct Mapper* mapper);
} Mapper;

void load_mapper(const char* filename, struct Mapper* mapper);
void free_mapper(struct Mapper* mapper);

void load_UXROM(struct Mapper* mapper);
void load_MMC1(struct Mapper* mapper);
void load_CNROM(struct Mapper* mapper);

#endif //OLDNES_MAPPER_H
