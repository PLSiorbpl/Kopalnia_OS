#pragma once
#include "std/types.hpp"

namespace framebuffer {
    struct framebuffer_info {
        u32 width;
        u32 height;
        u32 pitch;
        u64 pixel_width;
    };

    class framebuffer {
    public:
        framebuffer() = default;
        ~framebuffer() = default;
        void init();

        void swap();
        void clear(u32 color);
        void set_pixel(u32 x, u32 y, u32 color) const;

        void put_char_at(char c, u32 x, u32 y, u32 color);
        void put_char(char c, u32 color);

        void scroll();
    private:
        void inc_cursor(const i32 amount) {
            cursor_x += amount;
            if (cursor_x >= width_in_chars) {
                cursor_x = 0;
                cursor_y++;
                if (cursor_y >= height_in_chars) {
                    scroll();
                    cursor_y = height_in_chars - 1;
                }
            }
        }

        void dec_cursor(const i32 amount) {
            cursor_x -= amount;
            if (cursor_x < 0) {
                cursor_x = width_in_chars - 1;
                if (cursor_y > 0) {
                    cursor_y--;
                }
            }
        }

        bool initialized = false;
        bool is_dirty = true;
        i32 height_in_chars = 0;
        i32 width_in_chars = 0;

        i32 cursor_x = 0;
        i32 cursor_y = 0;
        u32* back_buffer {};
        u32* front_buffer {};
        framebuffer_info info {};
    };
}