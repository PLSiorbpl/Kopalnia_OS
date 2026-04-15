#pragma once
#include "types.hpp"
#include "std/string.h"

namespace term {
    // VGA video buffer
    extern volatile uint16_t * video;
    extern int cursor_x;
    extern int cursor_y;

    // Consts
    constexpr int VGA_WIDTH = 80;
    constexpr int VGA_HEIGHT = 25;
    constexpr int TAB_SIZE = 4;

    enum class Color : uint8_t {
        Black = 0,
        Blue = 1,
        Green = 2,
        Cyan = 3,
        Red = 4,
        Magenta = 5,
        Brown = 6,
        LightGray = 7,
        DarkGray = 8,
        LightBlue = 9,
        LightGreen = 10,
        LightCyan = 11,
        LightRed = 12,
        Pink = 13,
        Yellow = 14,
        White = 15
    };

    void scroll();
    void print(const char* text, Color color = Color::White);

    template <typename T>
    void print_number(T value, Color color = Color::LightGreen) {
        char buffer[16];
        std::to_str(buffer, value);
        print(buffer, color);
    }

    void print_hex(uint32_t value, Color color = Color::LightGreen);

    void put_char(char c, Color color = Color::LightCyan);
    void clear(Color BGcolor = Color::Black);

    void print_serial(const char* text);
}
