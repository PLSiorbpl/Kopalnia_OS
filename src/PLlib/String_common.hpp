#pragma once
#include "types.hpp"

namespace string {
    int strlen(const char* text);
    void int_to_str(char* buffer, int value);
    void int_to_strhex(char* buffer, int value);
    bool str_cmp(const char* str1, const char* str2);
}

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
    void print_int(int value, Color color = Color::LightGreen);
    void print_hex(int value, Color color = Color::LightGreen);
    void put_char(char c, Color color = Color::LightCyan);
    void clear(Color BGcolor = Color::Black);
}
