#pragma once

namespace std {
    bool str_cmp(const char* str1, const char* str2);
    int strlen(const char* text);

    template <typename T> struct is_floating_point             { static const bool value = false; };
    template <>           struct is_floating_point<float>      { static const bool value = true;  };
    template <>           struct is_floating_point<double>     { static const bool value = true;  };
    template <>           struct is_floating_point<long double>{ static const bool value = true;  };

    template <typename T>
    static const bool is_floating_point_v = is_floating_point<T>::value;

    template <typename T>
    void to_str(char* buffer, T value) {
        if constexpr (is_floating_point_v<T>) {
            int i = 0;
            bool is_negative = false;

            int int_part = static_cast<int>(value);
            double frac_part = value - int_part;

            if (frac_part < 0)
                frac_part = -frac_part;
            int precision = 6;

            if (int_part < 0) {
                int_part = -int_part;
                is_negative = true;
            }

            if (int_part == 0) {
                buffer[i++] = '0';
            } else {
                while (0 < int_part) {
                    const auto digit = int_part % 10;
                    buffer[i++] = '0' + digit;
                    int_part /= 10;
                }
            }

            if (is_negative) {
                buffer[i++] = '-';
            }

            for (int j = 0; j < i / 2; j++) {
                const char tmp = buffer[j];
                buffer[j] = buffer[i - j - 1];
                buffer[i - j - 1] = tmp;
            }

            buffer[i++] = '.';
            for (int d = 0; d < precision; d++) {
                frac_part *= 10;
                int digit = static_cast<int>(frac_part);
                buffer[i++] = '0' + digit;
                frac_part -= digit;
            }

            buffer[i] = '\0';
        }
        else {
            int i = 0;
            bool is_negative = false;

            if (value < 0) {
                value = -value;
                is_negative = true;
            }

            if (value == 0) {
                buffer[i++] = '0';
            } else {
                while (0 < value) {
                    const auto digit = value % 10;
                    buffer[i++] = '0' + digit;
                    value /= 10;
                }
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

}
