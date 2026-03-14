#pragma once
#include "PLlib/types.hpp"

namespace x86 {
    void outb(uint16_t port, uint8_t val);
    void safe_outb(uint16_t port, uint8_t val);

    uint8_t inb(uint16_t port);

    void io_wait();

    void halt();

    void set_INT_flag(bool set);
}