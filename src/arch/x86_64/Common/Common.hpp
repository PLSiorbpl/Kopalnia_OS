#pragma once
#include "../../../libs/std/types.hpp"

namespace x64 {
    inline void outb(uint16_t port, uint8_t val) {
        asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
    }
    inline void outl(uint16_t port, uint32_t val) {
        asm volatile("outl %0, %1" : : "a"(val), "Nd"(port));
    }

    inline void io_wait() {
        outb(0x80, 0);
    }

    inline void outb_safe(const uint16_t port, const uint8_t val) {
        outb(port, val);
        io_wait();
    }

    inline uint8_t inb(uint16_t port) {
        uint8_t ret;
        asm volatile ("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
        return ret;
    }
    inline uint32_t inl(uint16_t port) {
        uint32_t ret;
        asm volatile("inl %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
        return ret;
    }

    inline void halt() {
        asm volatile("sti; hlt");
    }

    void set_INT_flag(bool flag);
    void pic_send_eoi(uint8_t irq);
}