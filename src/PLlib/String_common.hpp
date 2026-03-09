#pragma once
#include "types.hpp"

namespace string {
    int strlen(const char* text);
    void int_to_str(char* buffer, int value);
    bool str_cmp(const char* str1, const char* str2);
}

namespace term {
    // VGA video buffer
    inline auto video = reinterpret_cast<volatile uint16_t * const>(0xB8000);
    inline int cursor_x = 0;
    inline int cursor_y = 0;
    // Consts
    constexpr int VGA_WIDTH = 80;
    constexpr int VGA_HEIGHT = 25;
    constexpr int TAB_SIZE = 4;

    void scroll(int amount);
    void print(const char* text);
    void print_int(int value);
    void put_char(char c);
    void clear();
}
