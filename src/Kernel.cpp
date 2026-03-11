#include "PLlib/String_common.hpp"
#include "Drivers/Keyboard.hpp"
#include "PLlib/mem_common.hpp"

extern "C" [[noreturn]] void kernel_main() {
    term::print("------------ Kopalnia OS ------------\n");
    term::print("new line!\n");
    term::print("Lol it works!\n");
    term::print("Really long line ------------------------------------------------------------------------------------------------------------------------------------------ lol\n");
    for (char i = 0; i < 5;i++) {
        term::print_int(i*10);
        term::print("\n");
    }
    term::print("Text\n");
    term::print("TAB Te\tst \\t (4 spaces)\n");
    term::print("Update!!:\tAdded 3 very simple commands (will change in future) :3\n");
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
                        term::print("\tcommands: help, clear, echo\n");
                    } else if (string::str_cmp(buffer, "clear")) {
                        term::clear();
                    } else if (string::str_cmp(buffer, "echo")) {
                        term::print("\techo: too lazy to make arguments xD\n");
                    } else {
                        term::print("\tUnknown command\n");
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