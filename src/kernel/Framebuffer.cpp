#include "Framebuffer.hpp"

#include <libs/String_common.hpp>

#include "multiboot2.hpp"
#include "kernel/Memory/heap.hpp"
#include "../libs/std/mem_common.hpp"

namespace Framebuffer {
    uint32_t* back_buffer;
    uint32_t* front_buffer;
    info Info;

    void Init() {
        if (!Multiboot::Frame_buffer->addr) return;
        Info.width = Multiboot::Frame_buffer->width;
        Info.height = Multiboot::Frame_buffer->height;
        Info.bpp = Multiboot::Frame_buffer->bpp;
        Info.pitch = Multiboot::Frame_buffer->pitch;
        Info.Size_B = Info.height * Info.pitch;

        back_buffer = static_cast<uint32_t *>(heap::malloc(Info.Size_B));
    }

    void Swap() {
        for (uint32_t y = 0; y < Info.height; y++) {
            mem::memcpy(reinterpret_cast<uint32_t *>(Multiboot::Frame_buffer->addr + y * Info.pitch),
                        back_buffer + y * Info.pitch, Info.pitch);
        }
    }

    void Clear(const uint32_t color) {
        for (uint32_t y = 0; y < Info.height; y++) {
            mem::memset32(back_buffer + y * Info.pitch, color, Info.width);
        }
    }
}
