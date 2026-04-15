#include "system.hpp"

#include "kernel/Memory/heap.hpp"
#include "arch/x86_64/IDT/IDT.hpp"
#include "arch/x86_64/Common/Common.hpp"
#include "kernel/Sleep.hpp"
#include "kernel/Paging.hpp"

namespace systemPL {
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
        Paging::Map_memory(0x0, 1024*1024*32);
        //Paging::Map_memory(USB::base, USB::base+USB::size, Paging::Profile::MMIO);

        Paging::Enable_paging();

        x64::set_INT_flag(true); // Enable interrupts

        //USB::Init();
    }
}
