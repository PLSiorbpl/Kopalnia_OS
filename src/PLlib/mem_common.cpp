#include "mem_common.hpp"

#include "types.hpp"

namespace mem {
    volatile void* memcpy(volatile void* dst, volatile const void* src, const size_t size) {
        auto d = static_cast<volatile uint8_t *>(dst);
        const auto s = static_cast<volatile const uint8_t *>(src);
        for (size_t i = 0; i < size; i++) {
            d[i] = s[i];
        }
        return dst;
    }

    volatile void* memset(volatile void* dst, const uint8_t value, const size_t size) {
        auto d = static_cast<volatile uint8_t *>(dst);
        for (size_t i = 0; i < size; i++) {
            d[i] = value;
        }
        return dst;
    }

    volatile uint16_t *memset16(volatile uint16_t *dst, const uint16_t value, const size_t count) {
        for (size_t i = 0; i < count; i++) {
            dst[i] = value;
        }
        return dst;
    }
}
