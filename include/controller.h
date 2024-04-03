#ifndef OLDNES_CONTROLLER_H
#define OLDNES_CONTROLLER_H

#include <SDL2/SDL.h>

#include "definitions.h"

typedef enum Button {
    RIGHT  = 1 << 7,
    LEFT   = 1 << 6,
    DOWN   = 1 << 5,
    UP     = 1 << 4,
    START  = 1 << 3,
    SELECT = 1 << 2,
    B      = 1 << 1,
    A      = 1,
} Button;

typedef struct Controller {
    byte status;
    byte index;
    byte strobe;
    byte player;
} Controller;

void init_controller(struct Controller* controller, byte player);
byte read_controller(struct Controller* controller);
void write_controller(struct Controller* controller, byte value);
void update_controller(struct Controller* controller, const SDL_Event* event);

void keyboard_mapper(struct Controller* controller, const SDL_Event* event);

#endif //OLDNES_CONTROLLER_H
