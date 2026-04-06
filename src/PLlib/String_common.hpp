#pragma once
#include "types.hpp"

namespace string {
    int strlen(const char* text);
    void int_to_str(char* buffer, int64_t value);
    void uint_to_str(char* buffer, uint64_t value);
    void int_to_strhex(char* buffer, int64_t value);
    bool str_cmp(const char* str1, const char* str2);
}

namespace term {
    // VGA video buffer
    extern uint16_t * video;
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
    void print_int(int64_t value, Color color = Color::LightGreen);
    void print_uint(uint64_t value, Color color = Color::LightGreen);
    void print_hex(int32_t value, Color color = Color::LightGreen);
    void put_char(char c, Color color = Color::LightCyan);
    void clear(Color BGcolor = Color::Black);

    void Serial_Write(const char* text);
#ifndef NDEBUG
    #define assert(condition) \
    do { \
        if (!(condition)) { \
            term::Serial_Write("Assert failed in: "); \
            term::Serial_Write(__FILE__); \
            term::Serial_Write(" at line: "); \
            char buf[12]; \
            string::int_to_str(buf, __LINE__); \
            term::Serial_Write(buf); \
            term::Serial_Write(" in function: "); \
            term::Serial_Write(__FUNCTION__); \
            term::Serial_Write("\n"); \
        } \
    } while (false);
#else
#define assert(condition) do {} while(false)
#endif
}
