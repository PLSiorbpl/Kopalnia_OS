#include "system.hpp"

#include "kernel/Memory/heap.hpp"
#include "arch/x86_64/IDT/IDT.hpp"
#include "arch/x86_64/Common/Common.hpp"
#include "Drivers/Keyboard.hpp"
#include "kernel/Sleep.hpp"
#include "kernel/Paging.hpp"
#include "kernel.h"

namespace systemPL {
    extern "C" char user_stack_top;
    extern "C" char stack_top;
    extern "C" u64 kernel_rsp;

    extern "C" void handle_syscall();

    void Init(void* mbi) {
        //Multiboot::Init(static_cast<uint8_t *>(mbi));
        Paging::Init(); // 4KB page size

        // GDT is done in gdt.asm and elevate.asm
        IDT::IDT_Install();

        // Timer frequency
        Time::Set_PIT(100); // 100Hz

        // Heap Initialization
        heap::heap_init(1024*1024*32);

        //USB::PreInit();

        // Paging
        Paging::Map_memory(0x0, 1024*1024*32, User);
        //Paging::Map_memory(USB::base, USB::base+USB::size, Paging::Profile::MMIO);

        Paging::Enable_paging();

        kb::flush_keyboard();

        x64::set_INT_flag(true); // Enable interrupts

        //USB::Init();

        kernel_rsp = reinterpret_cast<u64>(&stack_top);

        // enter user space
        asm volatile(
            ".intel_syntax noprefix\n"

            "mov ecx, 0xC0000082\n"
            "mov rax, %0\n"
            "mov rdx, %1\n"
            "wrmsr\n"

            "mov ecx, 0xC0000080\n"
            "rdmsr\n"
            "or eax, 1\n"
            "wrmsr\n"

            "mov ecx, 0xC0000081\n"
            "rdmsr\n"
            "mov edx, %4\n"
            "wrmsr\n"

            "mov rcx, %2\n"
            "mov r11, 0x202\n"
            "mov rsp, %3\n"

            "and rsp, ~0xF\n"
            "sub rsp, 8\n"

            "mov ax, 0x2B\n"
            "mov ds, ax\n"
            "mov es, ax\n"
            "mov fs, ax\n"
            "mov gs, ax\n"

            "sysretq\n"

            ".att_syntax prefix\n"
            :
            : "r"(reinterpret_cast<u64>(handle_syscall) & 0xFFFFFFFF),
              "r"(reinterpret_cast<u64>(handle_syscall) >> 32),
              "r"(reinterpret_cast<u64>(user_space_main)),
              "r"(reinterpret_cast<u64>(&user_stack_top)),
              "r"(static_cast<u32>(0x00200008))
            : "rcx", "r11", "rax", "rdx", "memory"
        );
    }
}

