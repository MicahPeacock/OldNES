#include "emulator.h"

static void handle_event(struct Emulator* emulator, const SDL_Event* event);

void init_emulator(struct Emulator* emulator, int argc, char* argv[]) {
    if (argc < 2) {
        exit(EXIT_FAILURE);
    }
    load_mapper(argv[1], &emulator->mapper);

    init_memory(emulator);
    init_cpu(emulator);

    emulator->exit  = 0;
    emulator->pause = 0;
}

void free_emulator(struct Emulator* emulator) {
    free_mapper(&emulator->mapper);
}

void run_emulator(struct Emulator* emulator) {
    struct CPU* const cpu = &emulator->cpu;

    SDL_Event event;
    while (!emulator->exit) {
        while (SDL_PollEvent(&event)) {
            handle_event(emulator, &event);
        }
        if (!emulator->pause) {

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