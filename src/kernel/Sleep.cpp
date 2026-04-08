#include "Sleep.hpp"

#include <arch/x86_64/Common/Common.hpp>
#include "arch/x86_64/IDT/PIT.hpp"

namespace Time {
    uint64_t hz;
    volatile uint64_t tick = 0;
    // 1000 -> 1s 500 -> 0.5s
    void Sleep(const uint64_t t) {
        const uint64_t end = tick + ((t * hz) / 1000);
        while (tick < end) {
            x64::halt();
        }
    }

    // Sets timer frequency
    void Set_PIT(const uint64_t freq) {
        x64::set_INT_flag(false);
        PIT::timer_set_frequency(freq);
        x64::set_INT_flag(true);
        hz = freq;
    }
}
