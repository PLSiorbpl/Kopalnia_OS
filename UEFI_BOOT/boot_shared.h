#pragma once

struct Framebuffer {
    void* base;
    __UINT64_TYPE__ size;
    __UINT32_TYPE__ width;
    __UINT32_TYPE__ height;
    __UINT32_TYPE__ pixels_per_scanline;
};

extern Framebuffer framebuffer_internal;