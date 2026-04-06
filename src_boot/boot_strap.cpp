#include "types.hpp"
#include "x86/GDT/GDT.hpp"
#include "Paging/Paging.hpp"

extern "C" void Boot_strap(uint32_t magic, void* mbi) {
    auto* video = reinterpret_cast<volatile uint16_t* const>(0xB8000);
    video[1] = (0x07 << 8) | 'H';
    //GDT::gdt_install();

    //heap::heap_init(1024*1024*4); // 4MB
    //Paging::Init();
    //Paging::Map_memory(0x0, 1024*1024*4);
    //Paging::Enable_paging();

    //IDT::idt_install();
    //PIT::pit_init(100); // 100hz

    //x86::set_INT_flag(true); // enable interrupts

    // Long jump to 64bit kernel or smt
}