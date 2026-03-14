#include "Keyboard.hpp"

#include <arch/x86/Common/common.hpp>
#include "PLlib/types.hpp"

namespace kb {
    // force get char (waits until char)
    char get_char() {
        while (buf.empty())
            x86::halt(); // so it doesnt use 100% cpu

        uint8_t sc;
        buf.pop(sc);
        if (sc < sizeof(scancode_map)/sizeof(scancode_map[0]))
            return scancode_map[sc];
        return 0;
    }
}