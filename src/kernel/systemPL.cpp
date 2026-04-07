#include "systemPL.hpp"

#include <String_common.hpp>

#include "Memory/heap.hpp"
#include "kernel/multiboot2.hpp"
#include "arch/x86_64/IDT/IDT.hpp"

namespace systemPL {
    void Init(void* mbi) {
        //Multiboot::Init(static_cast<uint8_t *>(mbi));

        // GDT is done in gdt.asm and elevate.asm
        IDT::IDT_Install();

        // Timer frequency
        //Time::Set_PIT(100); // 100Hz

        // Heap Initialization
        //heap::heap_init(1024*1024*32);

        // Paging and PAT
        //Paging::Init(); // 4KB page size
        //Paging::Map_memory(0x0, 1024*1024*32);
        //Paging::Map_memory(IntelGPU::GPU_MMIO_BASE, IntelGPU::GPU_MMIO_BASE + 32*1024*1024);
        //Paging::Map_memory(Multiboot::Frame_buffer->addr, Multiboot::Frame_buffer->addr+(Multiboot::Frame_buffer->pitch*Multiboot::Frame_buffer->height));
        //Paging::Enable_paging();

        asm volatile("sti");

        //x86::set_INT_flag(true); // Enable interrupts
    }
}
