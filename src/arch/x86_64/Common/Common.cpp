#include "Common.hpp"

namespace x64 {
    void set_INT_flag(bool flag) {
        if (flag)
            asm volatile("sti");
        else
            asm volatile("cli");
    }

    void pic_send_eoi(const uint8_t irq) {
        if (irq >= 8) {
            outb(0xA0, 0x20);
        }
        outb(0x20, 0x20);
    }
}
