#include <SDL2/SDL.h>

#include "emulator.h"

static void handle_event(struct Emulator* emulator, const SDL_Event* event);

void init_emulator(struct Emulator* emulator, int argc, char* argv[]) {
    if (argc < 2) {
        exit(EXIT_FAILURE);
    }
    load_mapper(argv[1], &emulator->mapper);

    init_ppu_bus(emulator);
    init_cpu_bus(emulator);
    init_ppu(emulator);
    init_cpu(emulator);

    struct GraphicsContext* gfx = &emulator->gfx;
    gfx->width  = NES_VIDEO_WIDTH;
    gfx->height = NES_VIDEO_HEIGHT;
    gfx->scale  = 3.0f;
    init_graphics(gfx, SDL_INIT_EVERYTHING);

    emulator->exit  = 0;
    emulator->pause = 0;
}

void free_emulator(struct Emulator* emulator) {
    free_graphics(&emulator->gfx);
    free_mapper(&emulator->mapper);
}

void run_emulator(struct Emulator* emulator) {
    struct CPU* cpu = &emulator->cpu;
    struct PPU* ppu = &emulator->ppu;
    struct Controller* pad1 = &emulator->cpu_bus.pad1;
    struct Controller* pad2 = &emulator->cpu_bus.pad2;
    struct GraphicsContext* gfx = &emulator->gfx;

    SDL_Event event;
    while (!emulator->exit) {
        while (SDL_PollEvent(&event)) {
            update_controller(pad1, &event);
            update_controller(pad2, &event);
            handle_event(emulator, &event);
        }
        if (!emulator->pause) {
            while (!ppu->render) {
                execute_ppu(ppu);
                execute_ppu(ppu);
                execute_ppu(ppu);
                execute_cpu(cpu);
            }
            render_graphics(gfx, ppu->screen_buffer);
            ppu->render = 0;
        } else {

        }
    }
}

static void handle_event(struct Emulator* emulator, const SDL_Event* event) {
    switch (event->type) {
        case SDL_KEYDOWN: {
            switch (event->key.keysym.sym) {
                case SDLK_ESCAPE:
                    emulator->exit = 1;
                    break;
                case SDLK_SPACE:
                    emulator->pause ^= 1;
                    break;
                default:
                    break;
            }
            break;
        }
        case SDL_QUIT: {
            emulator->exit = 1;
            break;
        }
        default: {

        }
    }
}