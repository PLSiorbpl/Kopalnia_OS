#include "String_common.hpp"

#include "types.hpp"
#include "mem_common.hpp"

namespace string {
    bool str_cmp(const char *str1, const char *str2) {
        size_t idx = 0;
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

    void int_to_str(char* buffer, int value) {
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

    void int_to_strhex(char *buffer, const int value) {
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
    void scroll(const int amount) {
        for (int i = amount; i > 0; i--) {
            // Copy Line-1 to Line
            for (int y = 0; y < VGA_HEIGHT - 1; y++) {
                mem::memcpy(&video[y * VGA_WIDTH], &video[(y+1)*VGA_WIDTH], VGA_WIDTH * sizeof(uint16_t));
            }
            // Clear last line
            mem::memset16(&video[(VGA_HEIGHT-1) * VGA_WIDTH], (0x07 << 8) | ' ', VGA_WIDTH);
            cursor_y--;
            if (cursor_y < 0) cursor_y = 0;
        }
    }

    void print(const char* text, Color color) {
        for (int i = 0; text[i] != '\0'; i++) {
            const char c = text[i];
            const int idx = (cursor_y*VGA_WIDTH)+cursor_x;

            // Special cases
            if (c == '\t') { // TAB
                cursor_x += TAB_SIZE;
                if (cursor_x >= VGA_WIDTH) {
                    cursor_x = 0;
                    cursor_y+=1;
                }
                continue;
            } else if (c == '\n') { // Enter
                cursor_x = 0;
                cursor_y+=1;
                continue;
            } else if (c == '\b') { // Backspace
                cursor_x -= 1;
                video[idx-1] = (0x07 << 8) | ' ';
                if (cursor_x < 0) {
                    cursor_x = VGA_WIDTH;
                    cursor_y -= 1;
                    if (cursor_y < 0) cursor_y = 0;
                }
                continue;
            }
            // Scroll
            if (cursor_y >= VGA_HEIGHT) {
                scroll(1);
            }

            // Draw char
            video[idx] = (static_cast<uint8_t>(color) << 8) | c;
            cursor_x+=1;

            // New line (if end of current)
            if (cursor_x >= VGA_WIDTH) {
                cursor_x = 0;
                cursor_y+=1;
            }
        }
    }

    void print_int(const int value, const Color color) {
        char buf[12];
        string::int_to_str(buf, value);
        print(buf, color);
    }

    void print_hex(const int value, const Color color) {
        char buf[12];
        string::int_to_strhex(buf, value);
        print(buf, color);
    }

    void put_char(const char c, const Color color) {
        const char buf[2] = {c, '\0'};
        print(buf, color);
    }

    // Clear whole screen
    void clear(Color BGcolor) {
        mem::memset16(video, (static_cast<uint8_t>(BGcolor) << 8) | ' ', VGA_WIDTH*VGA_HEIGHT);
        cursor_x = 0;
        cursor_y = 0;
    }
}
