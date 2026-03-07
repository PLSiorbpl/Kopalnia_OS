#include "PLlib/String_common.cpp"

extern "C" void kernel_main() {
    term::print("------------ Kobylnik OS ------------\n");
    term::print("new line!\n");
    term::print("Lol it works!\n");
    term::print("Really long line ------------------------------------------------------------------------------------------------------------------------------------------ lol\n");
    for (char i = 0; i < 15;i++) {
        term::print_int(i);
        term::print("\n");
    }
    term::print("Text\n");
    term::print("TAB Te\tst \\t (4 spaces)\n");

    while (true) {
    }
}