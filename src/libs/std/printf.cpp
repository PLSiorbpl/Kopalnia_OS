#include "printf.hpp"
#include "libs/String_common.hpp"

namespace std {
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
                        put_char(static_cast<char>(__builtin_va_arg(args, int)), active_color);
                        break;
                    case 's':
                        print(__builtin_va_arg(args, const char*), active_color);
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
                            term::put_serial(static_cast<char>(__builtin_va_arg(args, int)));
                            break;
                        case 's':
                            term::print_serial(__builtin_va_arg(args, const char*));
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
                            term::put_serial(c);
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
                            term::put_serial(c);
                            continue;
                        }
                    }
                    i++;
                    continue;
                }

                term::put_serial(c);
            }

            __builtin_va_end(args);
        }
    }
}