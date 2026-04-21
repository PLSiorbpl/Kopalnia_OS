#pragma once
#include "../libs/std/types.hpp"

namespace drivers::vga::cursor {
    extern int cursor_x;
    extern int cursor_y;

    void enable_cursor(u8 cursor_start, u8 cursor_end);
    void disable_cursor();

    void dec_cursor(int amount);
    void inc_cursor(int amount);

    void update_cursor();
}
