#include "String_common.hpp"

#include <arch/x86_64/Common/Common.hpp>

#include "types.hpp"
#include "mem_common.hpp"

namespace string {
    bool str_cmp(const char *str1, const char *str2) {
        uint64_t idx = 0;
        while (str1[idx] == str2[idx]) {
            if (str1[idx] == '\0') return true;
            idx++;
        }
        return false;
    }

    int strlen(const char* text) {
        int len = 0;
        while (text[len] != '\0') {
            len++;
        }
        return len;
    }

    void int_to_str(char* buffer, int64_t value) {
        int i = 0;
        bool is_negative = false;

        if (value == 0) {
            buffer[0] = '0';
            buffer[1] = '\0';
            return;
        }
        if (value < 0) {
            is_negative = true;
            value = -value;
        }

        while (value > 0) {
            const int digit = value % 10;
            buffer[i] = '0' + digit;
            i += 1;
            value /= 10;
        }

        if (is_negative) {
            buffer[i] = '-';
            i += 1;
        }
        buffer[i] = '\0';

        for (int j = 0; j < i/2; j++) {
            const char tmp = buffer[j];
            buffer[j] = buffer[i-1-j];
            buffer[i-1-j] = tmp;
        }
    }

    void uint_to_str(char* buffer, uint64_t value) {
        int i = 0;

        if (value == 0) {
            buffer[0] = '0';
            buffer[1] = '\0';
            return;
        }

        while (value > 0) {
            const int digit = value % 10;
            buffer[i] = '0' + digit;
            i += 1;
            value /= 10;
        }

        buffer[i] = '\0';

        for (int j = 0; j < i/2; j++) {
            const char tmp = buffer[j];
            buffer[j] = buffer[i-1-j];
            buffer[i-1-j] = tmp;
        }
    }

    void int_to_strhex(char *buffer, const int64_t value) {
        auto hex_chars = "0123456789ABCDEF";

        buffer[0] = '0';
        buffer[1] = 'x';

        char *ptr = buffer + 2;

        for (int i = 28; i >= 0; i -= 4) {
            const uint8_t nibble = (value >> i) & 0xF;
            *ptr++ = hex_chars[nibble];
        }

        *ptr = '\0';
    }
}

namespace term {
    volatile uint16_t* video = reinterpret_cast<volatile uint16_t* const>(0xB8000);
    int cursor_x = 0;
    int cursor_y = 0;

    void scroll() {
        mem::memmove(video, video + 80, 24 * 80 * 2);
        mem::memset16(&video[(VGA_HEIGHT-1) * VGA_WIDTH], (0x07 << 8) | ' ', VGA_WIDTH);
    }

    void print(const char* text, Color color) {
        x64::set_INT_flag(false);
        for (int i = 0; text[i] != '\0'; i++) {
            const char c = text[i];

            // Safety check
            if (cursor_x >= VGA_WIDTH) {
                cursor_x = 0;
                cursor_y+=1;
            }
            if (cursor_y >= VGA_HEIGHT) {
                scroll();
                cursor_y = VGA_HEIGHT - 1;
            }

            // Special cases
            if (c == '\t') { // TAB
                cursor_x += TAB_SIZE;
                continue;
            } else if (c == '\n') { // Enter
                cursor_x = 0;
                cursor_y+=1;
                continue;
            } else if (c == '\b') { // Backspace
                if (cursor_x > 0) {
                    cursor_x--;
                } else if (cursor_y > 0) {
                    cursor_y--;
                    cursor_x = VGA_WIDTH - 1;
                }
                const int idx = cursor_y * VGA_WIDTH + cursor_x;
                video[idx] = (0x07 << 8) | ' ';
                continue;
            }

            // Draw char
            const int idx = cursor_y*VGA_WIDTH+cursor_x;

            video[idx] = (static_cast<uint8_t>(color) << 8) | c;

            cursor_x+=1;
        }
        x64::set_INT_flag(true);
    }

    void print_int(const int64_t value, const Color color) {
        char buf[16];
        string::int_to_str(buf, value);
        print(buf, color);
    }

    void print_uint(const uint64_t value, const Color color) {
        char buf[16];
        string::uint_to_str(buf, value);
        print(buf, color);
    }

    void print_hex(const uint32_t value, const Color color) {
        char buf[16];
        string::int_to_strhex(buf, value);
        print(buf, color);
    }

    void put_char(const char c, const Color color) {
        const char buf[2] = {c, '\0'};
        print(buf, color);
    }

    // Clear whole screen
    void clear(Color BGcolor) {
        x64::set_INT_flag(false);
        mem::memset16(video, (static_cast<uint8_t>(BGcolor) << 8) | ' ', VGA_WIDTH*VGA_HEIGHT);
        x64::set_INT_flag(true);
        cursor_x = 0;
        cursor_y = 0;
    }

    void Serial_Write(const char *text) {
        for (int i = 0; text[i] != '\0'; i++) {
            while (!(x64::inb(0x3F8 + 5) & 0x20)) {}
            x64::outb(0x3F8, text[i]);
        }
    }
}
