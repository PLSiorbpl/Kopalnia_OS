#pragma once
#include "types.hpp"

namespace mem {
    volatile void* memcpy(volatile void* dst, volatile const void* src, size_t size);
    void* memmove(void* dst, const void* src, size_t size);

    void* memset(void* dst, uint8_t value, size_t size);

    uint16_t *memset16(uint16_t *dst, uint16_t value, size_t count);
    uint32_t *memset32(uint32_t *dst, uint32_t value, size_t count);
}