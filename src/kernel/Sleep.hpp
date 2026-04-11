#pragma once
#include "PLlib/types.hpp"

namespace Time {
    extern uint64_t hz;
    extern volatile uint64_t tick;

    void Sleep(uint64_t t);
    bool WaitForKey(uint64_t t, char key);

    void Set_PIT(uint64_t freq);
}