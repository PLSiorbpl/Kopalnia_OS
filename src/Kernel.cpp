#include "kernel/systemPL.hpp"
#include "PLlib/String_common.hpp"
#include "Drivers/Keyboard.hpp"
#include "PLlib/mem_common.hpp"
#include "kernel/Sleep.hpp"
#include "kernel/Memory/heap.hpp"

extern "C" [[noreturn]] void kernel_main() {
    systemPL::Init();
    term::print("------------ Kopalnia OS ------------\n\n", term::Color::Green);
    term::print("Update!!:\tInterrupts (Timer and Keyboard)\n\n", term::Color::Yellow);

    term::print("malloc() test!\n", term::Color::LightBlue);
    void* adr = heap::malloc(1);
    auto adr_val = reinterpret_cast<uintptr_t>(adr);
    term::print_hex(static_cast<int>(adr_val));
    term::print("\nKernel size: ");
    term::print_int(reinterpret_cast<uintptr_t>(&heap::_end) - 0x100000);
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
                        term::print("\tcommands: help, clear, echo, poweroff (VM Only), sleep\n", term::Color::LightBlue);
                    } else if (string::str_cmp(buffer, "clear")) {
                        term::clear();
                    } else if (string::str_cmp(buffer, "echo")) {
                        term::print("\techo: too lazy to make arguments xD\n", term::Color::Pink);
                    } else if (string::str_cmp(buffer, "poweroff")) {
                        term::print("\tShutting down in 2.5s", term::Color::LightRed);
                        Time::Sleep(2.5);
                        asm volatile("outw %0, %1" : : "a"(static_cast<uint16_t>(0x2000)), "Nd"(static_cast<uint16_t>(0x604)));
                    } else if (string::str_cmp(buffer, "sleep")) {
                        term::print("\tSleeping for 5 seconds\n", term::Color::LightGreen);
                        Time::Sleep(5);
                    } else {
                        term::print("\tUnknown command\n", term::Color::LightGray);
                        Time::Sleep(0.25);
                    }

                    term::print("Kopalnia-OS>");
                    mem::memset(buffer, 0, 256);
                    i = 0;
                }
            }
            if (c == '\b' && i >= 255)
                i--;
        }
    }
}