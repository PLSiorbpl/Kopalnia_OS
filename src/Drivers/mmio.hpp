#pragma once
#include "../libs/std/types.hpp"

namespace MMIO {
    class mmio {
    public:
        uint64_t base;

        uint64_t MMIO_READ64(const uint32_t reg) const {
            const auto ptr = reinterpret_cast<volatile uint64_t *>(base + reg);
            return *ptr;
        }

        void MMIO_WRITE64(const uint32_t reg, const uint64_t value) const {
            *reinterpret_cast<volatile uint64_t *>(base + reg) = value;
        }

        uint32_t MMIO_READ32(const uint32_t reg) const {
            const auto ptr = reinterpret_cast<volatile uint32_t *>(base + reg);
            return *ptr;
        }

        void MMIO_WRITE32(const uint32_t reg, const uint32_t value) const {
            *reinterpret_cast<volatile uint32_t *>(base + reg) = value;
        }

        uint16_t MMIO_READ16(const uint32_t reg) const {
            const auto ptr = reinterpret_cast<volatile uint16_t *>(base + reg);
            return *ptr;
        }

        void MMIO_WRITE16(const uint32_t reg, const uint16_t value) const {
            *reinterpret_cast<volatile uint16_t *>(base + reg) = value;
        }

        uint8_t MMIO_READ8(const uint32_t reg) const {
            const auto ptr = reinterpret_cast<volatile uint8_t *>(base + reg);
            return *ptr;
        }

        void MMIO_WRITE8(const uint32_t reg, const uint8_t value) const {
            *reinterpret_cast<volatile uint8_t *>(base + reg) = value;
        }
    };
}