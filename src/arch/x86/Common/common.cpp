#include "common.hpp"
#include "PLlib/types.hpp"

namespace x86 {
    void outb(uint16_t port, uint8_t val) {
        asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
    }

    void safe_outb(uint16_t port, uint8_t val) {
        asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
        outb(0x80, 0);
    }

    uint8_t inb(uint16_t port) {
        uint8_t ret;
        asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }

    void io_wait() {
        outb(0x80, 0);
    }

    void halt() {
        asm volatile("hlt");
    }

    // Set Interrupt flag (true - sti, false - cli)
    void set_INT_flag(const bool set) {
        if (set)
            asm volatile("sti");
        else
            asm volatile("cli");
    }
}
