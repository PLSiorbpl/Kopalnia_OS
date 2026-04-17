#include "gdt.h"

constexpr uint64_t gdt_entry(const uint32_t base, const uint32_t limit, const uint8_t access, const uint8_t flags)
{
    return
        (static_cast<uint64_t>(limit & 0x0000FFFF) << 0)   |
        (static_cast<uint64_t>(base & 0x00FFFFFF) << 16)   |
        (static_cast<uint64_t>(access)              << 40) |
        (static_cast<uint64_t>(limit & 0x000F0000) << 32)  |
        (static_cast<uint64_t>(flags & 0x0F)       << 52)  |
        (static_cast<uint64_t>(base >> 24)          << 56);
}

extern "C" {
    extern char stack_top;

    tss_entry tss = {
        .rsp0 = reinterpret_cast<u64>(&stack_top),
    };

    __attribute__((aligned(8)))
    uint64_t gdt[] = {
        0,
        gdt_entry(0, 0xFFFFF, ACCESS_PRESENT | ACCESS_RING0 | ACCESS_CODE_SEG | ACCESS_READABLE, FLAG_GRANULARITY | FLAG_32BIT), // 0x08
        gdt_entry(0, 0xFFFFF, ACCESS_PRESENT | ACCESS_RING0 | ACCESS_DATA_SEG | ACCESS_WRITABLE, FLAG_GRANULARITY | FLAG_32BIT), // 0x10
        gdt_entry(0, 0, ACCESS_PRESENT | ACCESS_RING0 | ACCESS_CODE_SEG | ACCESS_READABLE, FLAG_64BIT), // 0x18
        gdt_entry(0, 0, ACCESS_PRESENT | ACCESS_RING0 | ACCESS_DATA_SEG | ACCESS_WRITABLE, FLAG_64BIT), // 0x20
        gdt_entry(0, 0, ACCESS_PRESENT | ACCESS_RING3 | ACCESS_CODE_SEG | ACCESS_READABLE, FLAG_64BIT), // 0x28
        gdt_entry(0, 0, ACCESS_PRESENT | ACCESS_RING3 | ACCESS_DATA_SEG | ACCESS_WRITABLE, FLAG_64BIT), // 0x30
        0, // 0x38, TSS low
        0, // 0x40, TSS high, both filled by asm
    };

    gdt_descriptor gdt_descriptor = {
        .limit = sizeof(gdt) - 1,
        .base = 0,
    };
}