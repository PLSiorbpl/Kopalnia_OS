#pragma once
#include "types.hpp"

namespace mem {
    void* memcpy(void* dst, const void* src, uint64_t size);
    volatile void* memmove(volatile void* dst, volatile const void* src, uint64_t size);

    void* memset(void* dst, uint8_t value, uint64_t size);

    volatile uint16_t *memset16(volatile uint16_t *dst, uint16_t value, uint64_t count);
    uint32_t *memset32(uint32_t *dst, uint32_t value, uint64_t count);
}