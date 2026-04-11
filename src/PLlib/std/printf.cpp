#include "printf.hpp"
#include "PLlib/types.hpp"
#include "arch/x86_64/Common/Common.hpp"
#include "PLlib/String_common.hpp"

namespace std {
#define State_Normal 0
#define State_Type 1

    void printf(const char* text, ...) {
        __builtin_va_list args;
        __builtin_va_start(args, text);

        uint8_t STATE = State_Normal; // Normal state
        for (int i = 0; text[i] != '\0'; i++) {
            const char c = text[i];
            const char c2 = text[i+1];
            char c3 = 0;
            if (c2 != '\0')
                c3 = text[i+2];

            // Args
            if (c == '%' && STATE == State_Normal) {
                STATE = State_Type;
                continue;
            }
            if (STATE == State_Type) {
                if (c == 'i') {
                    if (c2 == '6' && c3 == '4') {
                        term::print_int(__builtin_va_arg(args, long long)); // int64_t
                        i += 2;
                    } else {
                        term::print_int(__builtin_va_arg(args, int)); // int32_t
                    }
                } else if (c == 'u') {
                    if (c2 == '6' && c3 == '4') {
                        term::print_uint(__builtin_va_arg(args, unsigned long long)); // uint64_t
                        i += 2;
                    } else {
                        term::print_uint(__builtin_va_arg(args, unsigned int)); // uint32_t
                    }
                } else if (c == 's') {
                    term::print(__builtin_va_arg(args, const char*)); // const char*
                } else if (c == 'x') {
                    term::print_hex(__builtin_va_arg(args, unsigned int)); // uint32_t
                } else if (c == 'c') {
                    term::put_char(static_cast<char>(__builtin_va_arg(args, int)), term::Color::White); // char (int)
                } else if (c == '%') {
                    term::put_char(c, term::Color::White);
                } else {
                    term::put_char('%');
                    term::put_char(c, term::Color::White);
                }
                STATE = State_Normal;
                continue;
            }
            term::put_char(c, term::Color::White);
        }
        __builtin_va_end(args);
    }
}