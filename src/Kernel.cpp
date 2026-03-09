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
        if (c) {
            term::put_char(c);
            buffer[i] = c;
            i++;
            // Logic
            if (c == '\n') {
                buffer[i] = '\0';
                // Do command:
                if (string::str_cmp(buffer, "help\n")) {
                    term::print("\tcommands: help, clear, echo\n");
                } else if (string::str_cmp(buffer, "clear\n")) {
                    term::clear();
                } else if (string::str_cmp(buffer, "echo\n")) {
                    term::print("\techo: too lazy to make arguments xD\n");
                } else {
                    term::print("\tUnknown command\n");
                }

                term::print("Kopalnia-OS>");
                mem::memset(buffer, 0, 256);
                i = 0;
            }
        }
    }
}