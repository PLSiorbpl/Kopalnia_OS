#include "kernel_entry.h"

#include "kernel/system.hpp"
#include "libs/limine.h"

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3)

__attribute__((used, section(".limine_requests")))
static volatile limine_framebuffer_request fb_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

extern "C" void setup();

extern "C" void kernel_main() {
    if (LIMINE_BASE_REVISION_SUPPORTED == false)
        for(;;) __asm__("hlt");

    if (fb_request.response == nullptr || fb_request.response->framebuffer_count < 1)
        for(;;) __asm__("hlt");

    setup();

    auto* fb = fb_request.response->framebuffers[0];

    framebuffer::framebuffer_info fb_info {
        .base = fb->address,
        .width = static_cast<u32>(fb->width),
        .height = static_cast<u32>(fb->height),
        .pixels_in_scanline = static_cast<uint32_t>(fb->pitch / 4),
        .size = fb->pitch * fb->height
    };

    auto* memmap = memmap_request.response;
    uint64_t heap_addr = 0;
    uint64_t heap_size = 8 * 1024 * 1024; // 8MB

    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        auto* entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= heap_size) {
            heap_addr = entry->base + hhdm_request.response->offset;
            break;
        }
    }

    if (!heap_addr)
        for(;;) __asm__("hlt");


    systemPL::Init(fb_info, heap_addr);

    volatile uint32_t *fb_ptr = static_cast<volatile uint32_t *>(fb->address);
    for (size_t y = 0; y < fb->height; y++) {
        for (size_t x = 0; x < fb->width; x++) {
            uint32_t nX = x * 255 / fb->width;
            uint32_t nY = y * 255 / fb->height;
            fb_ptr[y * (fb->pitch / 4) + x] = (nY << 8) | nX;
        }
    }

    for(;;) __asm__("hlt");
}
