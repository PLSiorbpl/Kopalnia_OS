#pragma once
#include "Drivers/achi/ahci.h"
#include "Drivers/fs/partition/partition_manager.h"
#include "Drivers/GPU/framebuffer.hpp"
#include "std/types.hpp"

namespace systemPL {
    extern "C" char user_stack_top;
    extern "C" char stack_top;
    extern "C" u64 kernel_rsp;

    extern "C" void enter_user_space();

    extern "C" void handle_syscall();

    extern drivers::ahci::ahci ahci;
    extern framebuffer::framebuffer fb;
    extern fs::partition::partition_manager partition_manager;

    void Init(void* mbi);
}