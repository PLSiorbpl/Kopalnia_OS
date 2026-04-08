#include "PLlib/types.hpp"
#include "PLlib/String_common.hpp"
#include "PLlib/mem_common.hpp"

#include "kernel/systemPL.hpp"
#include "kernel/Sleep.hpp"
#include "kernel/Memory/heap.hpp"

#include "Drivers/Keyboard.hpp"
#include "Drivers/PCI.hpp"

extern "C" void kernel_main(uint32_t magic, void* mbi) {
    //x86::outb(0x3C2, 0x00);
    systemPL::Init(mbi);
    //Framebuffer::Init();
    //Framebuffer::Clear(0x00ff00ff);
    //Framebuffer::Swap();

    term::print("------------ Kopalnia OS 64bit ------------\n\n", term::Color::Green);

    term::print("Kernel size: ");
    term::print_uint(reinterpret_cast<uint64_t>(&heap::_end - heap::start_));
    term::print("B\n\n");

    term::print("Kopalnia-OS>");

    static char buffer[256];
    static int i = 0;
    while (true) {
        const char c = kb::get_char();

        if (c == '\b' && i <= 0)
            continue;

        if (c) {
            if (i < 255) {
                term::put_char(c);
                if (c == '\b') {
                    i -= 1;
                    buffer[i] = ' ';
                } else {
                    buffer[i] = c;
                    i += 1;
                }
                // Logic
                if (c == '\n') {
                    // override \n with \0
                    if (i > 0)
                        buffer[i-1] = '\0';
                    else
                        buffer[0] = '\0';

                    // Do commands:
                    if (string::str_cmp(buffer, "help")) {
                        term::print("\tcommands: help, clear, echo, poweroff (VM Only), sleep, heap, PCI\n", term::Color::LightBlue);
                    } else if (string::str_cmp(buffer, "heap")) {
                        heap::dump_heap();
                    } else if (string::str_cmp(buffer, "clear")) {
                        term::clear();
                    } else if (string::str_cmp(buffer, "pci")) {
                        PCI::Test();
                    } else if (string::str_cmp(buffer, "echo")) {
                        term::print("\techo: too lazy to make arguments xD\n", term::Color::Pink);
                    } else if (string::str_cmp(buffer, "poweroff")) {
                        term::print("\tShutting down in 2.5s", term::Color::LightRed);
                        Time::Sleep(2500);
                        asm volatile("outw %0, %1" : : "a"(static_cast<uint16_t>(0x2000)), "Nd"(static_cast<uint16_t>(0x604)));
                    } else if (string::str_cmp(buffer, "sleep")) {
                        term::print("\tSleeping for 5 seconds\n", term::Color::LightGreen);
                        Time::Sleep(5000);
                    } else {
                        term::print("\tUnknown command\n", term::Color::LightGray);
                        Time::Sleep(250);
                    }

                    term::print("Kopalnia-OS>");
                    mem::memset(buffer, 0, 256);
                    i = 0;
                }
            }
            if (c == '\b' && i >= 255) {
                i--;
                term::print("\b");
            }
        }
    }
}