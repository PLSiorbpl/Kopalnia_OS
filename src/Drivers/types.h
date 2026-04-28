#pragma once
#include "std/types.hpp"

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

inline u32 color_to_rgb(Color c) {
    static const u32 palette[16] = {
        0x000000, // Black
        0x0000AA, // Blue
        0x00AA00, // Green
        0x00AAAA, // Cyan
        0xAA0000, // Red
        0xAA00AA, // Magenta
        0xAA5500, // Brown
        0xAAAAAA, // LightGray
        0x555555, // DarkGray
        0x5555FF, // LightBlue
        0x55FF55, // LightGreen
        0x55FFFF, // LightCyan
        0xFF5555, // LightRed
        0xFF55FF, // Pink
        0xFFFF55, // Yellow
        0xDFDFDF, // White
    };
    return palette[static_cast<u8>(c)];
}