#pragma once

namespace std {
    bool str_cmp(const char* str1, const char* str2);
    int strlen(const char* text);

    template <typename T>
    void to_str(char* buffer, T value) {
        int i = 0;
        bool is_negative = false;

        if (value < 0) {
            value = -value;
            is_negative = true;
        }

        while (0 < value) {
            const auto digit = value % 10;
            buffer[i++] = '0' + digit;
            value /= 10;
        }

        if (is_negative) {
            buffer[i++] = '-';
        }

        for (int j = 0; j < i / 2; j++) {
            const char tmp = buffer[j];
            buffer[j] = buffer[i - j - 1];
            buffer[i - j - 1] = tmp;
        }

        buffer[i] = '\0';
    }

}
