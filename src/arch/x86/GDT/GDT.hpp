#pragma once
#include "PLlib/types.hpp"

namespace GDT {
    struct GDTEntry {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t  base_middle;
        uint8_t  access;
        uint8_t  granularity;
        uint8_t  base_high;
    } __attribute__((packed));

    struct GDTPtr {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed));

    inline GDTEntry gdt[3];
    inline GDTPtr gdt_ptr;

    void gdt_set_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

    void gdt_install();
}