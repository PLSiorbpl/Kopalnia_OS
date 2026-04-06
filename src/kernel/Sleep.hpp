#pragma once
#include "PLlib/types.hpp"

namespace Time {
    extern uint64_t hz;

    void Sleep(double t);

    void Set_PIT(uint64_t freq);
}