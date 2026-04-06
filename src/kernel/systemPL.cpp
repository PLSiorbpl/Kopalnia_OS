#include "systemPL.hpp"

//#include <arch/x86/Common/common.hpp>
//#include <arch/x86/GDT/GDT.hpp>
#include "Memory/heap.hpp"
#include "kernel/multiboot2.hpp"

namespace systemPL {
    void Init(void* mbi) {
        // GDT and IDT Initialization
        //Multiboot::Init(static_cast<uint8_t *>(mbi));

        //GDT::gdt_install();
        //IDT::idt_install();

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


        //x86::set_INT_flag(true); // Enable interrupts
    }
}
