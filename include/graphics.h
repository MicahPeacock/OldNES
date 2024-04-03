#ifndef OLDNES_GRAPHICS_H
#define OLDNES_GRAPHICS_H

#include <SDL2/SDL.h>

typedef struct GraphicsContext {
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  texture;
    int width;
    int height;
    float scale;
} GraphicsContext;

void init_graphics(struct GraphicsContext* gfx, uint32_t systems);
void free_graphics(struct GraphicsContext* gfx);

void render_graphics(struct GraphicsContext* gfx, const uint32_t* buffer);

#endif //OLDNES_GRAPHICS_H
