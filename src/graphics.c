#include "graphics.h"
#include "log.h"

void init_graphics(struct GraphicsContext* gfx, uint32_t systems) {
    SDL_Init(systems);
    gfx->window = SDL_CreateWindow("OldNES Emulator",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            gfx->width * (int)gfx->scale,
            gfx->height * (int)gfx->scale,
            SDL_WINDOW_SHOWN);
    if (gfx->window == NULL) {
        LOG(ERROR, SDL_GetError());
        exit(EXIT_FAILURE);
    }

    gfx->renderer = SDL_CreateRenderer(gfx->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gfx->renderer == NULL) {
        LOG(ERROR, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_RenderSetLogicalSize(gfx->renderer, gfx->width, gfx->height);
    SDL_RenderSetIntegerScale(gfx->renderer, 1);
    SDL_RenderSetScale(gfx->renderer, gfx->scale, gfx->scale);

    gfx->texture = SDL_CreateTexture(gfx->renderer,
            SDL_PIXELFORMAT_ABGR8888,
            SDL_TEXTUREACCESS_STREAMING,
            gfx->width,
            gfx->height);
    if(gfx->texture == NULL){
        LOG(ERROR, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
    SDL_RenderClear(gfx->renderer);
    SDL_RenderPresent(gfx->renderer);

    LOG(DEBUG, "Initialized SDL subsystem");
}

void free_graphics(struct GraphicsContext* gfx) {
    SDL_DestroyTexture(gfx->texture);
    SDL_DestroyRenderer(gfx->renderer);
    SDL_DestroyWindow(gfx->window);
    SDL_Quit();

    LOG(DEBUG, "Graphics clean up finished.");
}

void render_graphics(struct GraphicsContext* gfx, const uint32_t* buffer){
    SDL_RenderClear(gfx->renderer);
    SDL_UpdateTexture(gfx->texture, NULL, buffer, (int)(gfx->width * sizeof(uint32_t)));
    SDL_RenderCopy(gfx->renderer, gfx->texture, NULL, NULL);
    SDL_RenderPresent(gfx->renderer);
}