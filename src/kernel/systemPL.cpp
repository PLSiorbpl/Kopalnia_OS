#include "systemPL.hpp"

#include <arch/x86/Common/common.hpp>
#include <arch/x86/GDT/GDT.hpp>
#include <arch/x86/IDT/IDT.hpp>
#include "Sleep.hpp"
#include "Memory/heap.hpp"

namespace systemPL {
    void Init() {
        // GDT and IDT Initialization
        GDT::gdt_install();
        IDT::idt_install();

        // Timer frequency
        Time::Set_PIT(100); // 100Hz

        // Enable interrupts
        x86::set_INT_flag(true);

        // Heap Initialization (1MB)
        heap::heap_init(1024*1024);
    }
}
