#pragma once
#include "PLlib/Ring_Buffer.hpp"

namespace kb {
    extern mem::Ring_Buffer<uint8_t, 256> buf;

    static constexpr char scancode_map[128] = {
        //[2] = '1', [3] = '3'
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 0, 0,
        '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
        0, 0, ' '
        };

    char get_char();
    char read_char();
}
