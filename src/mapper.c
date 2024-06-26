#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL_rwops.h>

#include "mapper.h"
#include "log.h"

#define NES_MAGIC "NES\x1A"

typedef struct INESHeader {
    byte nes_id[4];
    byte prg_banks;
    byte chr_banks;
    byte flags6;
    byte flags7;
    byte prg_ram_size;
    byte flags9;
    byte flags10;
    byte unused[5];
} INESHeader;

static byte* read_rom_data(SDL_RWops* file, usize bytes);
static void select_mapper(struct Mapper* mapper);

static byte read_prg(const struct Mapper* mapper, word address);
static byte read_chr(const struct Mapper* mapper, word address);
static void write_prg(struct Mapper* mapper, word address, byte value);
static void write_chr(struct Mapper* mapper, word address, byte value);
static void scanline_irq(struct Mapper* mapper);

void load_mapper(const char* filename, struct Mapper* mapper) {
    SDL_RWops* file = SDL_RWFromFile(filename, "rb");
    if (file == NULL) {
        LOG(ERROR, "File '%s' is not found.", filename);
        exit(EXIT_FAILURE);
    }

    INESHeader header;
    memset(mapper, 0, sizeof(struct Mapper));
    SDL_RWread(file, &header, sizeof(INESHeader), 1);

    if (strncmp((char*)header.nes_id, NES_MAGIC, 4) != 0) {
        LOG(ERROR, "Invalid iNES file format.");
        exit(EXIT_FAILURE);
    }

    mapper->prg_banks = header.prg_banks;
    mapper->chr_banks = header.chr_banks;
    LOG(INFO, "PRG banks (16KB): %u", mapper->prg_banks);
    LOG(INFO, "CHR banks  (8KB): %u", mapper->chr_banks);

    mapper->mirroring = (header.flags6 & 0x08) ?: (header.flags6 & 0x01);
    mapper->mapper_id = (header.flags7 & 0xf0) | ((header.flags6 & 0xf0) >> 4);

    mapper->prg_rom = read_rom_data(file, 0x4000 * mapper->prg_banks);
    mapper->chr_rom = read_rom_data(file, 0x2000 * mapper->chr_banks);

    SDL_RWclose(file);

    select_mapper(mapper);
}

void free_mapper(struct Mapper* mapper) {
    if (mapper->prg_rom != NULL) {
        free(mapper->prg_rom);
    }
    if (mapper->chr_rom != NULL) {
        free(mapper->chr_rom);
    }
    LOG(DEBUG, "Mapper cleanup complete");
}

static byte* read_rom_data(SDL_RWops* file, usize bytes) {
    if (!bytes) {
        return NULL;
    } else {
        byte* ptr = malloc(bytes);
        SDL_RWread(file, ptr, bytes, 1);
        return ptr;
    }
}

static void select_mapper(Mapper* mapper) {
    mapper->read_prg     = read_prg;
    mapper->read_chr     = read_chr;
    mapper->write_prg    = write_prg;
    mapper->write_chr    = write_chr;
    mapper->scanline_irq = scanline_irq;

    switch (mapper->mapper_id) {
        case NROM:
            // Do nothing. Uses default methods
            break;
        default:
            LOG(ERROR, "Mapper %u not implemented", mapper->mapper_id);
            exit(EXIT_FAILURE);
    }
}

static byte read_prg(const struct Mapper* mapper, word address) {
    return mapper->prg_rom[address & 0x7fff];
}

static byte read_chr(const struct Mapper* mapper, word address) {
    return mapper->chr_rom[address];
}

static void write_prg(struct Mapper* mapper, word address, byte value) {
    LOG(DEBUG, "Attempted to write to PRG-ROM");
}

static void write_chr(struct Mapper* mapper, word address, byte value) {
    LOG(DEBUG, "Attempted to write to CHR-ROM");
}

static void scanline_irq(struct Mapper* mapper) {
    // Do nothing. This is to be implemented by other mappers.
}
