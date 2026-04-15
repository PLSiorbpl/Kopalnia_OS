#include "String_common.hpp"

#include <arch/x86_64/Common/Common.hpp>

#include "types.hpp"
#include "mem_common.hpp"

namespace term {
    volatile uint16_t* video = reinterpret_cast<volatile uint16_t* const>(0xB8000);
    int cursor_x = 0;
    int cursor_y = 0;

    void scroll() {
        mem::memmove(video, video + 80, 24 * 80 * 2);
        mem::memset16(&video[(VGA_HEIGHT-1) * VGA_WIDTH], (static_cast<uint16_t>(Color::LightGray) << 8) | ' ', VGA_WIDTH);
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
            cursor_x = amount - 1;
            cursor_y++;
            if (cursor_y >= VGA_HEIGHT) {
                scroll();
                cursor_y = VGA_HEIGHT - 1;
            }
        }
    }

    inline void put_char_at(const char c, const uint8_t x, const uint8_t y, const Color color) {
        const int idx = y * VGA_WIDTH + x;
        video[idx] = static_cast<uint8_t>(color) << 8 | c;
    }

    inline char get_char_at(const uint8_t x, const uint8_t y) {
        const int idx = y * VGA_WIDTH + x;
        return video[idx] & 0xFF;
    }

    void put_char(const char c, const Color color) {
        if (c == '\t') { // Tab
            inc_cursor(TAB_SIZE);
            put_char_at('\t', cursor_x, cursor_y, Color::LightGray);
            return;
        }
        if (c == '\n') { // Return
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= VGA_HEIGHT) {
                scroll();
                cursor_y = VGA_HEIGHT - 1;
            }

            return;
        }
        if (c == '\b') { // Backspace
            dec_cursor(1);
            if (get_char_at(cursor_x, cursor_y) == '\t') {
                dec_cursor(TAB_SIZE - 1);
            }

            put_char_at(' ', cursor_x, cursor_y, Color::LightGray);

            if (get_char_at(cursor_x, cursor_y) == 'a') {
                put_char_at('9', cursor_x, cursor_y, Color::LightGray);
            }

            return;
        }

        put_char_at(c, cursor_x, cursor_y, color);

        inc_cursor(1);
    }

    void print(const char* text, Color color) {
        for (int i = 0; text[i] != '\0'; i++) {
            put_char(text[i], color);
        }
    }

    void put_str(const char* text, Color color) {
        print(text, color);
        put_char('\n', color);
    }

    void print_hex(uint32_t value, Color color) {
        auto hex_chars = "0123456789ABCDEF";
        char buffer[16];

        buffer[0] = '0';
        buffer[1] = 'x';

        char *ptr = buffer + 2;

        for (int i = 28; i >= 0; i -= 4) {
            const uint8_t nibble = (value >> i) & 0xF;
            *ptr++ = hex_chars[nibble];
        }

        *ptr = '\0';

        print(buffer, color);
    }

    // Clear whole screen
    void clear(Color BGcolor) {
        mem::memset16(video, (static_cast<uint8_t>(BGcolor) << 8) | ' ', VGA_WIDTH * VGA_HEIGHT);
        cursor_x = 0;
        cursor_y = 0;
    }

    void Serial_Write(const char *text) { // ?????
        for (int i = 0; text[i] != '\0'; i++) {
            while (!(x64::inb(0x3F8 + 5) & 0x20)) {}
            x64::outb(0x3F8, text[i]);
        }
    }
}
