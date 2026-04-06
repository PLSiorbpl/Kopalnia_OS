#include "Sleep.hpp"

//#include <arch/x86/Common/common.hpp>
//#include "arch/x86/IDT/IDT.hpp"
//#include "arch/x86/IDT/PIT.hpp"

namespace Time {
    uint64_t hz;
    // Sleeps x seconds (eg. 0.25f -> 0.25s)
    void Sleep(const double t) {
        //const double end = IDT::tick + (t*100);
        //while (IDT::tick < end) {
            //x86::halt();
        //}
    }

    // Sets timer frequency
    void Set_PIT(const uint64_t freq) {
        //x86::set_INT_flag(false);
        //PIT::pit_init(freq);
        //x86::set_INT_flag(true);
    }
}
