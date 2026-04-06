#pragma once
#include "PLlib/types.hpp"

namespace Multiboot {
    struct multiboot2_info {
        uint32_t total_size;
        uint32_t reserved;
    };

    struct tag {
        uint16_t type;
        uint16_t flags;
        uint32_t size;
    } __attribute__((packed));

    struct fb_tag {
        uint32_t type;
        uint32_t size;
        uint64_t addr;
        uint32_t pitch;
        uint32_t width;
        uint32_t height;
        uint8_t bpp;
        uint8_t type2;
        uint16_t reserved;
    } __attribute__((packed));

    extern fb_tag* Frame_buffer;

    void Init(uint8_t* start);
}