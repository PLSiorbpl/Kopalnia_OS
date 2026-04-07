#pragma once
#include "PLlib/types.hpp"

namespace IDT {
    struct IDTR {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed));

    struct IDTEntry {
        uint16_t offset_low; // 0–15
        uint16_t selector; // 64b code segment
        uint8_t  ist;
        uint8_t  type_attr;
        uint16_t offset_mid; // 16–31
        uint32_t offset_high; // 32–63
        uint32_t zero;
    } __attribute__((packed));

    extern IDTEntry idt[256];
    // Handlers
    extern "C" void* isr_table[256];

	void set_IDT_entry(IDTEntry& entry, void* handler);
    void IDT_Install();

}