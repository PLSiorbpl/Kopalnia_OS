#include "Keyboard.hpp"
#include "PLlib/types.hpp"

namespace kb {
    char get_char() {
        uint8_t value;
        asm volatile("inb %1, %0" : "=a"(value) : "Nd"(0x64));
        if ((value & 1) == 1) {
            uint8_t scancode;
            asm volatile("inb %1, %0" : "=a"(scancode) : "Nd"(0x60));
            if (scancode & 0x80) return 0;
            return scancode_map[scancode];
        }
        return 0;
    }
}
