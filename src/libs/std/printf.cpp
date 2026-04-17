#include "printf.hpp"

#include "arch/x86_64/syscall/syscall.h"
#include "libs/String_common.hpp"

namespace std {
    namespace kernel {
        void printf(const char *text, ...) {
            __builtin_va_list args;
            __builtin_va_start(args, text);

            auto active_color = term::Color::White;
            for (int i = 0; text[i] != '\0'; i++) {
                const char c = text[i];

                if (c == '%') {
                    const char arg = text[i + 1];
                    switch (arg) {
                        case 'c':
                            term::put_char(static_cast<char>(__builtin_va_arg(args, int)), active_color);
                            break;
                        case 's':
                            term::print(__builtin_va_arg(args, const char*), active_color);
                            break;
                        case 'l': {
                            char buf[16];
                            std::to_str(buf, __builtin_va_arg(args, long long));
                            term::print(buf);
                            break;
                        }
                        case 'i':
                        case 'd': {
                            char buf[16];
                            std::to_str(buf, __builtin_va_arg(args, int));
                            term::print(buf);
                            break;
                        }
                        case 'u': {
                            char buf[16];
                            std::to_str(buf, __builtin_va_arg(args, unsigned int));
                            term::print(buf);
                            break;
                        }
                        case 'x':
                            // too lazy to port print hex to not use sys calls
                            break;
                        case 'f': {
                            char buf[16];
                            std::to_str(buf, __builtin_va_arg(args, double));
                            term::print(buf);
                        }
                        break;

                        default: {
                            term::put_char(c, active_color);
                            continue;
                        }
                    }

                    i++;
                    continue;
                }

                if (c == '&') {
                    const char arg = text[i + 1];
                    switch (arg) {
                        case '0':
                            active_color = term::Color::Black;
                            break;
                        case '1':
                            active_color = term::Color::Blue;
                            break;
                        case '2':
                            active_color = term::Color::Green;
                            break;
                        case '3':
                            active_color = term::Color::Cyan;
                            break;
                        case '4':
                            active_color = term::Color::Red;
                            break;
                        case '5':
                            active_color = term::Color::Magenta;
                            break;
                        case '6':
                            active_color = term::Color::Brown;
                            break;
                        case '7':
                            active_color = term::Color::LightGray;
                            break;
                        case '8':
                            active_color = term::Color::DarkGray;
                            break;
                        case '9':
                            active_color = term::Color::LightBlue;
                            break;
                        case 'a':
                            active_color = term::Color::LightGreen;
                            break;
                        case 'b':
                            active_color = term::Color::LightCyan;
                            break;
                        case 'c':
                            active_color = term::Color::LightRed;
                            break;
                        case 'd':
                            active_color = term::Color::Pink;
                            break;
                        case 'e':
                            active_color = term::Color::Yellow;
                            break;
                        case 'f':
                            active_color = term::Color::White;
                            break;

                        default: {
                            term::put_char(c, active_color);
                            continue;
                        }
                    }

                    i++;
                    continue;
                }

                term::put_char(c, active_color);
            }

            __builtin_va_end(args);
        }

        void print(const char *text, const term::Color color) {
            term::print(text, color);
        }

        void put_char(const char c, const term::Color color) {
            term::put_char(c, color);
        }
    }

    void printf(const char* text, ...) {
        __builtin_va_list args;
        __builtin_va_start(args, text);

        auto active_color = term::Color::White;
        for (int i = 0; text[i] != '\0'; i++) {
            const char c = text[i];

            if (c == '%') {
                const char arg = text[i + 1];
                switch (arg) {
                    case 'c':
                        std::put_char(static_cast<char>(__builtin_va_arg(args, int)), active_color);
                        break;
                    case 's':
                        std::print(__builtin_va_arg(args, const char*), active_color);
                        break;
                    case 'l':
                        print_number(__builtin_va_arg(args, long long), active_color);
                        break;
                    case 'i':
                    case 'd':
                        print_number(__builtin_va_arg(args, int), active_color);
                        break;
                    case 'u':
                        print_number(__builtin_va_arg(args, unsigned int), active_color);
                        break;
                    case 'x':
                        print_hex(__builtin_va_arg(args, unsigned int), active_color);
                        break;
                    case 'f':
                        print_number(__builtin_va_arg(args, double), active_color);
                        break;

                    default: {
                        std::put_char(c, active_color);
                        continue;
                    }
                }

                i++;
                continue;
            }

            if (c == '&') {
                const char arg = text[i + 1];
                switch (arg) {
                    case '0':
                        active_color = term::Color::Black;
                        break;
                    case '1':
                        active_color = term::Color::Blue;
                        break;
                    case '2':
                        active_color = term::Color::Green;
                        break;
                    case '3':
                        active_color = term::Color::Cyan;
                        break;
                    case '4':
                        active_color = term::Color::Red;
                        break;
                    case '5':
                        active_color = term::Color::Magenta;
                        break;
                    case '6':
                        active_color = term::Color::Brown;
                        break;
                    case '7':
                        active_color = term::Color::LightGray;
                        break;
                    case '8':
                        active_color = term::Color::DarkGray;
                        break;
                    case '9':
                        active_color = term::Color::LightBlue;
                        break;
                    case 'a':
                        active_color = term::Color::LightGreen;
                        break;
                    case 'b':
                        active_color = term::Color::LightCyan;
                        break;
                    case 'c':
                        active_color = term::Color::LightRed;
                        break;
                    case 'd':
                        active_color = term::Color::Pink;
                        break;
                    case 'e':
                        active_color = term::Color::Yellow;
                        break;
                    case 'f':
                        active_color = term::Color::White;
                        break;

                    default: {
                        std::put_char(c, active_color);
                        continue;
                    }
                }

                i++;
                continue;
            }

            std::put_char(c, active_color);
        }

        __builtin_va_end(args);
    }

    void print(const char* text, const term::Color color) {
        sys_write(text, static_cast<u64>(color));
    }

    void put_char(const char c, const term::Color color) {
        sys_put_char(c, static_cast<u64>(color));
    }

    namespace serial {
        void printf(const char *text, ...) {
            __builtin_va_list args;
            __builtin_va_start(args, text);

            for (int i = 0; text[i] != '\0'; i++) {
                const char c = text[i];

                if (c == '%') {
                    const char arg = text[i + 1];
                    switch (arg) {
                        case 'c':
                            put_char(static_cast<char>(__builtin_va_arg(args, int)));
                            break;
                        case 's':
                            print(__builtin_va_arg(args, const char*));
                            break;
                        case 'l':
                            term::print_number_serial(__builtin_va_arg(args, long long));
                            break;
                        case 'i':
                        case 'd':
                            term::print_number_serial(__builtin_va_arg(args, int));
                            break;
                        case 'u':
                            term::print_number_serial(__builtin_va_arg(args, unsigned int));
                            break;
                        case 'x':
                            term::print_hex_serial(__builtin_va_arg(args, unsigned int));
                            break;
                        case 'f':
                            term::print_number_serial(__builtin_va_arg(args, double));
                            break;

                        default: {
                            put_char(c);
                            continue;
                        }
                    }

                    i++;
                    continue;
                }

                // Discard color
                if (c == '&') {
                    const char arg = text[i + 1];
                    switch (arg) {
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                        case 'a':
                        case 'b':
                        case 'c':
                        case 'd':
                        case 'e':
                        case 'f':
                            break;

                        default: {
                            put_char(c);
                            continue;
                        }
                    }
                    i++;
                    continue;
                }

                put_char(c);
            }

            __builtin_va_end(args);
        }

        void print(const char* text) {
            sys_serial_write(text);
        }

        void put_char(char c) {
            sys_serial_put_char(c);
        }
    }
}