#pragma once
#include "std/types.hpp"

namespace systemPL {
    extern "C" char user_stack_top;
    extern "C" char stack_top;
    extern "C" u64 kernel_rsp;

    extern "C" void enter_user_space();

    extern "C" void handle_syscall();

    void Init(void* mbi);
}