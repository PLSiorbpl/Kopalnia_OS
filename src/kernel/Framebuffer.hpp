#pragma once
#include "../libs/std/types.hpp"

namespace Framebuffer {
    extern uint32_t* back_buffer;
    extern uint32_t* front_buffer;
    struct info {
        uint32_t width;
        uint32_t height;
        uint8_t bpp;
        uint32_t pitch;
        uint64_t Size_B;
    };

    extern info Info;

    void Init();

    void Swap();

    void Clear(uint32_t color);

    inline void PutPixel(const uint32_t x, const uint32_t y, const uint32_t color) {
        back_buffer[(y*Info.pitch)+x] = color;
    }
}