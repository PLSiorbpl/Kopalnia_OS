#include "libs/std/types.hpp"
#include "libs/String_common.hpp"
#include "libs/std/mem_common.hpp"
#include "std/printf.hpp"
#include "kernel/system.hpp"
#include "kernel/Sleep.hpp"
#include "kernel/Memory/heap.hpp"
#include "Drivers/Keyboard.hpp"
#include "Drivers/PCI.hpp"
#include "Drivers/USB/usb.hpp"

struct Command {
    const char* name;
    void (*func)();
};

void list_commands();

Command commands[9] = {
    {"help", list_commands},
    {"clear", [] {
        term::clear();
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
        Time::Sleep(5000);
    }},
    {"heap", [] {
        heap::dump_heap();
    }},
    {"pci", [] {
        PCI::Test();
    }},
    {"size", [] {
        auto size = reinterpret_cast<uint64_t>(&heap::_end - heap::start_);
        std::printf("&9\tKernel size: &a%i%s\n", size, std::format_size(size));
    }},
    {"usb", [] {
        USB::Test_Ports();
    }},
    {"colors", [] {
        std::printf("&0 &&00 &1 &&11 &2 &&22 &3 &&33 &4 &&44 &5 &&55 &6 &&66 &7 &&77 &8 &&88 &9 &&99 &a &&aa &b &&bb &c &&cc &d &&dd &e &&ee &f &&ff\n");
    }}
};

void list_commands() {
    std::printf("&9\tCommands: &9%s", commands[0].name);
    for (i32 i = 1; i < sizeof(commands) / sizeof(Command); ++i) {
        std::printf("&9, %s", commands[i].name);
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
    std::printf("&aPrintf(%/i %/u %/s %/x %/c %/u %/f) &c%i %u %s %x %c %u %f\n", -6767, 0, "LOL", 0x00000666, 'j', 0xffffffffff, 3.146767);
    std::printf("&f------------ &bPlum OS 64bit &f------------\n\n");
    std::printf("&aHello from user space!\n");

    list_commands();

    std::printf("&fPlum-OS> ");

    static char buffer[256];
    static int i = 0;
    while (true) {
        const char c = kb::get_char();

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
                    std::printf("&7\tUnknown command: &c%s \n", buffer);
                    Time::Sleep(250); // fake delay so people think lots of stuff is happening fr
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
