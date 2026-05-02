#include "system.hpp"

#include "linker_info.hpp"
#include "log.h"
#include "kernel/Memory/heap.hpp"
#include "arch/x86_64/IDT/IDT.hpp"
#include "arch/x86_64/Common/Common.hpp"
#include "Drivers/Keyboard.hpp"
#include "kernel/Sleep.hpp"
#include "kernel/Paging.hpp"
#include "arch/x86_64/gdt/gdt.h"
#include "Drivers/achi/ahci.h"
#include "Drivers/achi/ahci_device.h"
#include "Drivers/fs/partition/partition_manager.h"
#include "Drivers/GPU/framebuffer.hpp"

extern u64 kernel_address_vert;
extern u64 kernel_address_phys;
extern u64 hddm_offset;

namespace systemPL {
    drivers::ahci::ahci ahci;
    framebuffer::framebuffer fb;
    fs::partition::partition_manager partition_manager;

    void Init(framebuffer::framebuffer_info framebuffer, u64 heap_addr) {
        init_tss();

        //Multiboot::Init(static_cast<uint8_t *>(mbi));
        Paging::Init(); // 4KB page size

        // GDT is done in gdt.asm and elevate.asm
        IDT::IDT_Install();

        // Timer frequency
        Time::Set_PIT(100); // 100Hz

        // Heap Initialization
        heap::heap_init(1024*1024*8, heap_addr);
        Paging::Map_memory(heap_addr, heap_addr + 1024*1024*8, Paging::Profile::KernelData);

        // Paging
        Paging::Map_memory(0x0, 1024*1024*16, Paging::Profile::UserCode);

        u64 kernel_size = reinterpret_cast<u64>(&Linker::__kernel_end) - reinterpret_cast<u64>(&Linker::__kernel_start);
        Paging::Map_memory_vp(kernel_address_vert, kernel_address_phys, kernel_size, Paging::Profile::KernelCode);

        u64 stack_virt = reinterpret_cast<u64>(&Linker::stack_bottom);
        u64 stack_phys = stack_virt - hddm_offset;
        u64 stack_size = reinterpret_cast<u64>(&Linker::stack_top)
                       - reinterpret_cast<u64>(&Linker::stack_bottom);
        Paging::Map_memory_vp(stack_virt, stack_phys, stack_size, Paging::Profile::KernelStack);

        kb::flush_keyboard();

        Paging::Enable_paging();

        x64::set_INT_flag(true); // Enable interrupts

        fb.init(framebuffer);

        fb.swap();

        //USB::m_xhci_driver.init_device();
        //USB::m_xhci_driver.start_device();

        fb.swap();

        // ahci.init();
        // fb.swap();
        // for (int i = 0; i < 32; ++i) {
        //     auto device = ahci.request_device(i);
        //     if (!device.is_active())
        //         continue;
        //
        //     device.initialize();
        //     auto size = static_cast<double>(device.get_sector_count() * device.get_sector_size());
        //     log::info("Device info for device %i: \n"
        //                         "\tModel: %s\n"
        //                         "\tFirmware version: %s\n"
        //                         "\tSize: %f%s\n", i, device.get_model(), device.get_firmware(), size, std::format_size(size));
        //
        //     //auto buffer = static_cast<u16*>(heap::malloc_align(device.get_sector_size(), 4));
        //     //mem::memset(buffer, 0, device.get_sector_size());
        //     //const auto value = "Hello World!\n";
        //     //mem::memmove(buffer, value, std::strlen(value));
        //     //device.write(0, 1, buffer);
        //     //heap::free_align(buffer);
        //     //
        //     //buffer = static_cast<u16*>(heap::malloc_align(device.get_sector_size(), 4));
        //     //device.read(0, 1, buffer);
        //     //std::kernel::printf("Read output: %s", buffer);
        //     //heap::free_align(buffer);
        // }
        //
        // fb.swap();
        //
        // Time::Sleep(100);
        //
        // auto device = ahci.request_device(0);
        // partition_manager.init(device);
        //
        // fb.swap();

        kernel_rsp = reinterpret_cast<u64>(&Linker::stack_top);

        log::info("About to enter user space\n");
        log::info("About to enter user space\n");
        log::info("About to enter user space\n");
        fb.swap();
        log::info("About to enter user space\n");
        log::info("About to enter user space\n");
        log::info("About to enter user space\n");
        fb.swap();
        enter_user_space();
    }
}
