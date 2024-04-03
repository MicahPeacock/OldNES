#include "controller.h"

void init_controller(struct Controller* controller, byte player) {
    controller->status = 0;
    controller->index  = 0;
    controller->strobe = 0;
    controller->player = player;
}

byte read_controller(struct Controller* controller) {
    if (controller->index > 7)
        return 1;
    byte value = (controller->status & (1 << controller->index)) != 0;
    if (!controller->strobe)
        controller->index++;
    return value;
}

void write_controller(struct Controller* controller, byte value) {
    controller->strobe = value & 1;
    if (controller->strobe)
        controller->index = 0;
}

void update_controller(struct Controller* controller, const SDL_Event* event) {
    keyboard_mapper(controller, event);
}

void keyboard_mapper(struct Controller* controller, const SDL_Event* event) {
    word key = 0;
    switch (event->key.keysym.sym) {
        case SDLK_RIGHT:
            key = RIGHT;
            break;
        case SDLK_LEFT:
            key = LEFT;
            break;
        case SDLK_DOWN:
            key = DOWN;
            break;
        case SDLK_UP:
            key = UP;
            break;
        case SDLK_RETURN:
            key = START;
            break;
        case SDLK_RSHIFT:
            key = SELECT;
            break;
        case SDLK_z:
            key = A;
            break;
        case SDLK_x:
            key = B;
            break;
        default:
            break;
    }
    if (event->type == SDL_KEYUP)
        controller->status &= ~key;
    if (event->type == SDL_KEYDOWN)
        controller->status |= key;
}