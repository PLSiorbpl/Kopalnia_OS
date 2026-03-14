#include "IDT.hpp"
#include "PLlib/types.hpp"
#include "arch/x86/Common/common.hpp"

namespace IDT {
    void idt_set_entry(const int n, const uint32_t handler, const uint16_t sel, const uint8_t flags) {
        idt[n].offset_low  = handler & 0xFFFF;
        idt[n].selector    = sel;
        idt[n].zero        = 0;
        idt[n].type_attr   = flags;
        idt[n].offset_high = (handler >> 16) & 0xFFFF;
    }

    void idt_install() {
        idt_ptr.limit = sizeof(idt) - 1;
        idt_ptr.base  = reinterpret_cast<uint32_t>(&idt);

        // fill all entries with 0
        for (int i = 0; i < 256; i++) {
            idt_set_entry(i, reinterpret_cast<uint32_t>(isr_dummy), 0x08, 0x8E); // selector 0x08 = code segment
        }

        idt_set_entry(32, reinterpret_cast<uint32_t>(irq0_handler), 0x08, 0x8E); // Timer
        idt_set_entry(33, reinterpret_cast<uint32_t>(irq1_handler), 0x08, 0x8E); // Keyboard

        x86::set_INT_flag(false);
        asm volatile("lidt %0" : : "m"(idt_ptr));
        pic_remap();
    }

    void pic_remap() {
        const uint8_t a1 = x86::inb(PIC1_DATA);
        x86::io_wait();
        const uint8_t a2 = x86::inb(PIC2_DATA);
        x86::io_wait();

        x86::safe_outb(PIC1_COMMAND, ICW1_INIT);
        x86::safe_outb(PIC2_COMMAND, ICW1_INIT);
        x86::safe_outb(PIC1_DATA, 0x20); // offset IRQ0 = vector 32
        x86::safe_outb(PIC2_DATA, 0x28); // offset IRQ8 = vector 40
        x86::safe_outb(PIC1_DATA, 4);    // slave na IRQ2
        x86::safe_outb(PIC2_DATA, 2);    // identity
        x86::safe_outb(PIC1_DATA, ICW4_8086);
        x86::safe_outb(PIC2_DATA, ICW4_8086);

        x86::safe_outb(PIC1_DATA, a1);
        x86::safe_outb(PIC2_DATA, a2);
    }
}
