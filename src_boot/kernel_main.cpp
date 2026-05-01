#include "kernel/system.hpp"
#include "libs/limine.h"

extern "C" void setup();

// extern "C" void kernel_main(Framebuffer* fb) {
//     setup();
//     systemPL::Init(fb);
//
//     auto* screen = static_cast<unsigned int *>(fb->base);
//
//     for (unsigned int y = 0; y < fb->height; y++) {
//         for (unsigned int x = 0; x < fb->width; x++)
//             screen[y * fb->pixels_per_scanline + x] = 0x00FF0000;
//     }
//
//     while (true) {}
// }