#pragma once
#include "PLlib/types.hpp"
#include "Drivers/PCI.hpp"

namespace IntelGPU {
    extern PCI::PCI_Device gpu;
    extern uint32_t GPU_MMIO_BASE;

    //extern "C" const uint32_t kbl_guc_bin_size;
    //extern "C" const uint8_t kbl_dmc_bin[];

    //extern "C" const uint32_t kbl_dmc_bin_size;
    //extern "C" const uint8_t kbl_guc_bin[];

    inline uint32_t MMIO_READ(const uint32_t reg) {
        const auto ptr = reinterpret_cast<volatile uint32_t *>(GPU_MMIO_BASE + reg);
        return *ptr;
    }

    inline void MMIO_WRITE32(const uint32_t reg, const uint32_t value) {
        *reinterpret_cast<volatile uint32_t *>(GPU_MMIO_BASE + reg) = value;
    }

    void Init(uint64_t vram_addr, uint32_t width, uint32_t height, uint32_t pitch);

    void LoadFirmware();

    void SetupDMA(uint64_t VRAM_phys_addr, uint32_t VRAM_pitch, uint32_t VRAM_height);
}