#include "PIT.hpp"
#include "arch/x86_64/Common/Common.hpp"
#include "PLlib/types.hpp"

namespace PIT {
    void timer_set_frequency(const uint32_t hz) {
        const uint32_t divisor = 1193182 / hz;

        x64::outb(0x43, 0x36);

        x64::outb(0x40, static_cast<uint8_t>(divisor & 0xFF)); // Low byte
        x64::outb(0x40, static_cast<uint8_t>((divisor >> 8) & 0xFF)); // High byte
    }
}