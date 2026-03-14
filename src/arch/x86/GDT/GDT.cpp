#include "GDT.hpp"
#include "PLlib/types.hpp"

namespace GDT {
    void gdt_set_entry(const int num, const uint32_t base, const uint32_t limit, const uint8_t access, const uint8_t gran) {
        gdt[num].base_low = (base & 0xFFFF);
        gdt[num].base_middle = (base >> 16) & 0xFF;
        gdt[num].base_high = (base >> 24) & 0xFF;

        gdt[num].limit_low = (limit & 0xFFFF);
        gdt[num].granularity = (limit >> 16) & 0x0F;

        gdt[num].granularity |= gran & 0xF0;
        gdt[num].access = access;
    }

    void gdt_install() {
        gdt_ptr.limit = sizeof(gdt) - 1;
        gdt_ptr.base  = reinterpret_cast<uint32_t>(&gdt);

        gdt_set_entry(0, 0, 0, 0, 0); // Null segment
        gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
        gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment

        asm volatile("lgdt %0" : : "m" (gdt_ptr));
        asm volatile(
            "jmp $0x08, $gdt_flush\n"
            "gdt_flush:\n"
            "mov $0x10, %ax\n"
            "mov %ax, %ds\n"
            "mov %ax, %es\n"
            "mov %ax, %fs\n"
            "mov %ax, %gs\n"
            "mov %ax, %ss"
        );
    }
}
