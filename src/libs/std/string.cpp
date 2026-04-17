#include "string.h"
#include "types.hpp"

namespace std {
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

    void to_hex_str(char* buffer, unsigned int value) {
        buffer[0] = '0';
        buffer[1] = 'x';

        char* ptr = buffer + 2;

        bool started = false;
        for (int i = 28; i >= 0; i -= 4) {
            const auto hex_chars = "0123456789ABCDEF";
            const uint8_t nibble = (value >> i) & 0xF;

            if (!started) {
                if (nibble == 0)
                    continue;
                started = true;
            }

            *ptr++ = hex_chars[nibble];
        }

        if (!started) {
            *ptr++ = '0';
        }

        *ptr = '\0';
    }
}
