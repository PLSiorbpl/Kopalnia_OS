#include "cursor.h"
#include "vga.h"
#include "arch/x86_64/Common/Common.hpp"

namespace drivers::vga::cursor {
    int cursor_x;
    int cursor_y;

    void enable_cursor(const u8 cursor_start, const u8 cursor_end)
    {
        x64::outb(0x3D4, 0x0A);
        x64::outb(0x3D5, (x64::inb(0x3D5) & 0xC0) | cursor_start);

        x64::outb(0x3D4, 0x0B);
        x64::outb(0x3D5, (x64::inb(0x3D5) & 0xE0) | cursor_end);
    }


    void disable_cursor() {
        x64::outb(0x3D4, 0x0A);
        x64::outb(0x3D5, 0x20);
    }

    void dec_cursor(const int amount) {
        cursor_x -= amount;
        if (cursor_x < 0) {
            cursor_x = VGA_WIDTH - amount - 1;
            if (cursor_y > 0) {
                cursor_y--;
            }
        }
    }

    void inc_cursor(const int amount) {
        cursor_x += amount;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= VGA_HEIGHT) {
                scroll();
                cursor_y = VGA_HEIGHT - 1;
            }
        }
    }

    void update_cursor() {
        const u16 pos = cursor_y * VGA_WIDTH + cursor_x;

        x64::outb(0x3D4, 0x0F);
        x64::outb(0x3D5, static_cast<u8>(pos & 0xFF));
        x64::outb(0x3D4, 0x0E);
        x64::outb(0x3D5, static_cast<u8>(pos >> 8 & 0xFF));
    }
}
