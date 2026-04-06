#include "mem_common.hpp"

#include "types.hpp"

namespace mem {
    volatile void* memcpy(volatile void* dst, volatile const void* src, const size_t size) {
        auto d64 = static_cast<volatile uint64_t *>(dst);
        auto s64 = static_cast<volatile const uint64_t *>(src);
        size_t i = 0;
        for (; i+8 < size; i += 8) {
            *d64++ = *s64++;
        }

        auto d8 = reinterpret_cast<volatile uint8_t *>(d64);
        auto s8 = reinterpret_cast<volatile const uint8_t *>(s64);
        for (; i < size; i++) {
            *d8++ = *s8++;
        }
        return dst;
    }

    void* memmove(void* dst, const void* src, const size_t size) {
        const auto d = static_cast<uint8_t*>(dst);
        const auto s = static_cast<const uint8_t*>(src);

        if (d < s) {
            for (size_t i = 0; i < size; i++)
                d[i] = s[i];
        } else {
            for (size_t i = size; i > 0; i--)
                d[i-1] = s[i-1];
        }

        return dst;
    }

    void* memset(void* dst, const uint8_t value, const size_t size) {
        const auto d = static_cast<uint8_t *>(dst);
        for (size_t i = 0; i < size; i++) {
            d[i] = value;
        }
        return dst;
    }

    uint16_t *memset16(uint16_t *dst, const uint16_t value, const size_t count) {
        for (size_t i = 0; i < count; i++) {
            dst[i] = value;
        }
        return dst;
    }

    uint32_t* memset32(uint32_t* dst, const uint32_t value, const size_t count) {
        const uint64_t v = (static_cast<uint64_t>(value) << 32) | value;
        auto* d64 = reinterpret_cast<uint64_t *>(dst);

        size_t i = 0;

        for (; i + 2 <= count; i += 2)
            *d64++ = v;

        auto* d32 = reinterpret_cast<uint32_t *>(d64);

        for (; i < count; i++)
            *d32++ = value;

        return dst;
    }
}
