#pragma once
#include "std/types.hpp"

namespace glyph {
    extern const u8 font8x16[256 * 16];

    constexpr int FONT_W = 8;
    constexpr int FONT_H = 16;

    inline const u8* get(const char c) {
        return &font8x16[static_cast<u8>(c) * 16];
    }
}
