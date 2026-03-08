#pragma once
#include "types.hpp"
#include "mem_common.hpp"

namespace mem {
    volatile void* memcpy(volatile void* dst, volatile const void* src, size_t size);

    volatile void* memset(volatile void* dst, uint8_t value, size_t size);

    volatile uint16_t *memset16(volatile uint16_t *dst, uint16_t value, size_t count);
}