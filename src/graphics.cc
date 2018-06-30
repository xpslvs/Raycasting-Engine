/* graphics.cc */
#include <cstdint>
#include <cstring>
#include <cassert>
extern "C" {
# include <SDL2/SDL.h>
}
#include "graphics.hh"
#include "state.hh"

static SDL_Window   *window;
static SDL_Renderer *renderer;
static SDL_Texture  *texture;
static uint32_t     *vbuffer;

void graphics::init(int width, int height)
{
    assert(SDL_Init(SDL_INIT_EVERYTHING) != -1);

    window = SDL_CreateWindow(
        "Raycasting",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        0
    );
    assert(window != NULL);

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    assert(renderer != NULL);

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height 
    );
    assert(texture != NULL);
    
    assert(width > 0 && height > 0);
    vbuffer = new uint32_t[width * height];
    std::memset(vbuffer, 0, width * height * sizeof(uint32_t));
    state.width  = width;
    state.height = height;
}

void graphics::quit(void)
{
    delete[] vbuffer;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void graphics::set_pixel(int x, int y, uint32_t color)
{
    vbuffer[y * state.width + x] = color;
}

void graphics::clear(void)
{
    SDL_RenderClear(renderer);
}

void graphics::load(void)
{
    SDL_UpdateTexture(
        texture,
        NULL,
        vbuffer,
        state.width * sizeof(uint32_t)
    );
    SDL_RenderCopy(
        renderer,
        texture,
        NULL,
        NULL
    );
}

void graphics::refresh(void)
{
    SDL_RenderPresent(renderer);
}
