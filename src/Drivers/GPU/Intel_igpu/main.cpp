#include "main.hpp"

namespace IntelGPU {
    PCI::PCI_Device gpu;
    uint32_t GPU_MMIO_BASE;

    void Init(const uint64_t vram_addr, const uint32_t width, const uint32_t height, const uint32_t pitch) {
        //VRAM_phys_addr = vram_addr;
        //VRAM_width = width;
        //VRAM_height = height;
        //VRAM_pitch = pitch;

        //gpu = PCI::Find(0x8086, 0x5917);
        //GPU_MMIO_BASE = gpu.bar[0] & ~0xF;
        //term::print("GPU_MMIO_BASE = ");
        //term::print_uint(GPU_MMIO_BASE);
        //term::print("\n");
        //if (gpu.device_id != 0x5917) return;
    }
}
