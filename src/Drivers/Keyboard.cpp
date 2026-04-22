#include "Keyboard.hpp"

#include <arch/x86_64/Common/Common.hpp>
#include "../libs/std/types.hpp"
#include "kernel/Sleep.hpp"
#include "std/string.h"

namespace kb {
    mem::Ring_Buffer<uint8_t, 256> buf = {};

    /*
    Waits for char in buffer
     */
    key_code get_char() {
        while (buf.empty())
            x64::halt(); // so it doesnt use 100% cpu

        uint8_t sc;
        buf.pop(sc);
        return static_cast<key_code>(sc);
    }

    /*
    Reads char from buffer
    If there's no char (or char cant be mapped to map) it outputs 0
     */
    key_code read_char() {
        if (buf.empty())
            return key_code::KEY_NULL;

        uint8_t sc;
        buf.pop(sc);
        return static_cast<key_code>(sc);
    }

    char to_char(key_code code) {
        return ascii_map[static_cast<uint8_t>(code)];
    }

    void flush_keyboard() {
        // Read and discard all pending bytes in PS/2 output buffer
        while (x64::inb(0x64) & 0x01)
            x64::inb(0x60);
    }
}
