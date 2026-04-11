#include "Keyboard.hpp"

#include <arch/x86_64/Common/Common.hpp>
#include "PLlib/types.hpp"

namespace kb {
    mem::Ring_Buffer<uint8_t, 256> buf;

    /*
    Waits for char in buffer
     */
    char get_char() {
        while (buf.empty())
            x64::halt(); // so it doesnt use 100% cpu

        uint8_t sc;
        buf.pop(sc);
        if (sc < sizeof(scancode_map)/sizeof(scancode_map[0]))
            return scancode_map[sc];
        return 0;
    }

    /*
    Reads char from buffer
    If there's no char (or char cant be mapped to map) it outputs 0
     */
    char read_char() {
        if (buf.empty())
            return 0;

        uint8_t sc;
        buf.pop(sc);
        if (sc < sizeof(scancode_map)/sizeof(scancode_map[0]))
            return scancode_map[sc];
        return 0;
    }
}