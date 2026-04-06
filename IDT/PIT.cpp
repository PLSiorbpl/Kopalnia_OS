#include "PIT.hpp"
#include "types.hpp"
//#include "arch/x86/Common/common.hpp"

namespace PIT {
    void pit_init(const uint32_t freq) {
        //const uint32_t divisor = 1193182 / freq;
        //x86::outb(0x43, 0x36); // Square wave
        //x86::outb(0x40, divisor & 0xFF); // LSB
        //x86::outb(0x40, divisor >> 8); // MSB
    }
}
