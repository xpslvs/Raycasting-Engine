/* graphics.hh */
#ifndef GRAPHICS_HH
#define GRAPHICS_HH

#include <cstdint>

namespace graphics
{
    void init(int, int);
    void quit(void);
    void clear(void);
    void set_pixel(int, int, uint32_t);
    void load(void);
    void refresh(void);
}

#endif
