#pragma once
#include "PLlib/types.hpp"

namespace IDT {
    struct IDTR {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed));

    struct IDTEntry {
        uint16_t offset_low;  // Offset bits 0-15
        uint16_t selector;    // Code segment selector (e.g., 0x08 or 0x18)
        uint8_t  ist;         // IST index in TSS
        uint8_t  type_attr;   // Type and attributes (0x8E for Interrupt Gate)
        uint16_t offset_mid;  // Offset bits 16-31
        uint32_t offset_high; // Offset bits 32-63 (Must be 32 bits!)
        uint32_t zero;        // Reserved
    } __attribute__((packed));

    extern IDTEntry idt[256];
    // Handlers
    extern "C" void* isr_table[256];

	void set_IDT_entry(IDTEntry& entry, void* handler);
    void IDT_Install();
    void PIC_Remap(uint8_t offset1, uint8_t offset2);

}