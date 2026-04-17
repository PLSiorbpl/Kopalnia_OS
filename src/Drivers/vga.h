#pragma once
#include "types.h"
#include "std/types.hpp"

namespace drivers::vga {
    // VGA video buffer
    extern volatile uint16_t * video;
    extern int cursor_x;
    extern int cursor_y;

    // Consts
    constexpr int VGA_WIDTH = 80;
    constexpr int VGA_HEIGHT = 25;
    constexpr int TAB_SIZE = 4;

    void scroll();
    void print(const char* text, Color color);
    void put_char(char c, Color color);
    void clear(Color color);
}