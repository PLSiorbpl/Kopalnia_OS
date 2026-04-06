#include "multiboot2.hpp"

namespace Multiboot {
    fb_tag* Frame_buffer;

    void Init(uint8_t *start) {
        if (!start) return;
        const uint32_t total_size = *reinterpret_cast<uint32_t *>(start);

        uint8_t* ptr = start + 8; // header skip

        while (ptr < start + total_size) {
            const tag* t = reinterpret_cast<tag *>(ptr);

            if (t->type == 0)
                break;

            if (t->type == 8 && t->size >= sizeof(fb_tag)) {
                Frame_buffer = reinterpret_cast<fb_tag *>(ptr);
                //break;
            }

            ptr += (t->size + 7) & ~7;
        }
    }
}
