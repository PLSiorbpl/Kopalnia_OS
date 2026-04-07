#include "IDT.hpp"
#include "PLlib/String_common.hpp"

namespace IDT {
    IDTEntry idt[256];

    void set_IDT_entry(IDTEntry &entry, void *handler) {
        const auto addr = reinterpret_cast<uint64_t>(handler);
        entry.offset_low = addr & 0xffff;
        entry.selector = 0x18;
        entry.ist = 0;
        entry.type_attr = 0x8e;
        entry.offset_mid = (addr >> 16) & 0xffff;
        entry.offset_high = (addr >> 32) & 0xffff;
        entry.zero = 0;
    }

    void IDT_Install() {
        for (int i = 0; i < 255; i++) {
            set_IDT_entry(idt[i], isr_table[i]);
        }

        IDTR idtr{};
        idtr.limit = sizeof(idt) - 1;
        idtr.base  = reinterpret_cast<uint64_t>(&idt);
        asm volatile("lidt %0" : : "m"(idtr));
    }
}
