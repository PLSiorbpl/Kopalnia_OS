#include "arch/x86_64/syscall/syscall.h"
#include "libs/std/types.hpp"
#include "libs/std/mem_common.hpp"
#include "std/printf.hpp"
#include "kernel/system.hpp"
#include "kernel/Sleep.hpp"
#include "kernel/Memory/heap.hpp"
#include "Drivers/Keyboard.hpp"
#include "Drivers/PCI.hpp"
#include "Drivers/vga.h"
#include "Drivers/USB/usb.hpp"
#include "std/string.h"
#include "kernel/linker_info.hpp"
#include "std/vector.hpp"

struct Command {
    const char* name;
    void (*func)();
};

inline uint64_t range(void* a, void* b) {
    return reinterpret_cast<uint64_t>(b) - reinterpret_cast<uint64_t>(a);
}

void list_commands();

Command commands[9] = {
    {"help", list_commands},
    {"clear", [] {
        drivers::vga::clear(Color::Black);
    }},
    {"poweroff", [] {
        std::printf("&c\tShutting down in 5s (press ENTER to cancel!)\n");
        if (!Time::WaitForKey(5000, '\n')) {
            asm volatile("outw %0, %1" : : "a"(static_cast<uint16_t>(0x2000)), "Nd"(static_cast<uint16_t>(0x604))); // QEMU only
            std::printf("&4Unable to shut down try shutting down manually\n");
        } else {
            std::printf("&a\tShutdown Canceled!\n");
        }
    }},
    {"sleep", [] {
        std::printf("&a\tSleeping for &f5 &aseconds\n");
        sys_sleep(5000);
    }},
    {"heap", [] {
        heap::dump_heap();
    }},
    {"pci", [] {
        PCI::Test();
    }},
    {"size", [] {
        auto kernel_size = range(&Linker::__kernel_start, &Linker::__kernel_end);
        auto text_size = range(&Linker::__kernel_text_start, &Linker::__kernel_text_end);
        auto rodata_size = range(&Linker::__kernel_rodata_start, &Linker::__kernel_rodata_end);
        auto data_size = range(&Linker::__kernel_data_start, &Linker::__kernel_data_end);
        auto bss_size = range(&Linker::__kernel_bss_start, &Linker::__kernel_bss_end);
        auto stack_size = range(&Linker::stack_bottom, &Linker::stack_top);
        auto user_stack_size = range(&Linker::user_stack_bottom, &Linker::user_stack_top);

        auto kernel_code_size = text_size + rodata_size;

        std::printf("&9\t.text &7size: &a%u%s \t&9.rodata &7size: &a%u%s\n", std::Output::std_out, text_size, std::format_size(text_size), rodata_size, std::format_size(rodata_size));
        std::printf("&9\t.data &7size: &a%u%s \t&9.bss &7size: &a%u%s\n", std::Output::std_out, data_size, std::format_size(data_size), bss_size, std::format_size(bss_size));
        std::printf("&9\t.stack &7size: &a%u%s \t&9.user_stack &7size: &a%u%s\n", std::Output::std_out, stack_size, std::format_size(stack_size), user_stack_size, std::format_size(user_stack_size));
        std::printf("&b\tKernel Code &7size: &a%u%s\n\n", std::Output::std_out, kernel_code_size, std::format_size(kernel_code_size));
        std::printf("&e\tTotal kernel &7size: &a%u%s\n", std::Output::std_out, kernel_size, std::format_size(kernel_size));
    }},
    {"usb", [] {
    }},
    {"colors", [] {
        std::printf("&0 &&00 &1 &&11 &2 &&22 &3 &&33 &4 &&44 &5 &&55 &6 &&66 &7 &&77 &8 &&88 &9 &&99 &a &&aa &b &&bb &c &&cc &d &&dd &e &&ee &f &&ff\n");
    }}
};

void list_commands() {
    std::printf("&9\tCommands: &9%s", std::Output::std_out, commands[0].name);
    for (i32 i = 1; i < sizeof(commands) / sizeof(Command); ++i) {
        std::printf("&9, %s", std::Output::std_out, commands[i].name);
    }
    std::printf("\n");
}

extern "C" void kernel_main(uint32_t magic, void* mbi) {
    systemPL::Init(mbi);
    //Framebuffer::Init();
    //Framebuffer::Clear(0x00ff00ff);
    //Framebuffer::Swap();
}

extern "C" void user_space_main() {
    std::printf("&aPrintf(%/i %/u %/s %/x %/c %/u %/f) &c%i %u %s %x %c %u %f\n", std::Output::std_out, -6767, 0, "LOL", 0x00000666, 'j', 0xffffffffff, 3.146767);
    std::printf("&f------------ &bPlum OS 64bit &f------------\n\n");
    std::printf("&aHello from user space!\n");

    list_commands();

    std::printf("&fPlum-OS> ");

    static char buffer[256];
    static int i = 0;
    while (true) {
        const char c = sys_get_char();

        if (c == '\b' && i <= 0)
            continue;

        if (c) {
            if (i < 254) { // 254 so we allow space for \n
                std::put_char(c);
                if (c == '\b') {
                    i -= 1;
                    buffer[i] = ' ';
                } else {
                    buffer[i] = c;
                    i += 1;
                }
            }

            if (c == '\n') {
                // override \n with \0
                if (i > 0)
                    buffer[i - 1] = '\0';
                else
                    buffer[0] = '\0';

                bool found_command = false;
                for (int i = 0; i < sizeof(commands) / sizeof(Command); ++i) {
                    if (std::str_cmp(buffer, commands[i].name)) {
                        commands[i].func();
                        found_command = true;
                    }
                }
                if (!found_command) {
                    std::printf("&7\tUnknown command: &c%s \n", std::Output::std_out, buffer);
                    sys_sleep(250); // i like that delay :)
                }

                std::printf("&fPlum-OS> ");
                mem::memset(buffer, 0, 256);
                i = 0;
            }

            if (c == '\b' && i >= 254) {
                i--;
                std::put_char('\b');
            }
        }
    }
}
