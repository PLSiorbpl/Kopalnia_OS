#include "system.hpp"

#include "linker_info.hpp"
#include "kernel/Memory/heap.hpp"
#include "arch/x86_64/IDT/IDT.hpp"
#include "arch/x86_64/Common/Common.hpp"
#include "Drivers/Keyboard.hpp"
#include "kernel/Sleep.hpp"
#include "kernel/Paging.hpp"
#include "arch/x86_64/gdt/gdt.h"
#include "Drivers/cursor.h"
#include "Drivers/achi/ahci.h"
#include "Drivers/ata/ata.h"
#include "Drivers/USB/xHCI/xHCI.hpp"

namespace systemPL {
    drivers::ahci::ahci ahci;

    void Init(void* mbi) {
        init_tss();

        //Multiboot::Init(static_cast<uint8_t *>(mbi));
        Paging::Init(); // 4KB page size

        // GDT is done in gdt.asm and elevate.asm
        IDT::IDT_Install();

        // Timer frequency
        Time::Set_PIT(100); // 100Hz

        // Heap Initialization
        heap::heap_init(1024*1024*8);

        // Paging
        Paging::Map_memory(0x0, 1024*1024*16, Paging::Profile::UserCode);
        // kinda safer but needs clean build sometimes
        /*
        Paging::Map_memory(0x0, reinterpret_cast<uint64_t>(&Linker::__heap_start), Paging::Profile::UserCode);
        Paging::Map_memory(reinterpret_cast<uint64_t>(&Linker::__heap_start), reinterpret_cast<uint64_t>(&Linker::__heap_start)+(1024*1024*16),
            Paging::Profile::UserData); */

        Paging::Enable_paging();

        kb::flush_keyboard();

        x64::set_INT_flag(true); // Enable interrupts

        USB::m_xhci_driver.init_device();
        USB::m_xhci_driver.start_device();

        ahci.init();

        //drivers::ata::device(false);

        drivers::vga::cursor::enable_cursor(0, 15);

        kernel_rsp = reinterpret_cast<u64>(&stack_top);

        Time::Sleep(250);

        enter_user_space();
    }
}

