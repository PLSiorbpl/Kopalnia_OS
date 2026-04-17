#pragma once
#include "String_common.hpp"

namespace std {
    namespace kernel {
        void printf(const char* text, ...);
        void print(const char* text, term::Color color  = term::Color::LightGray);
        void put_char(char c, term::Color color  = term::Color::LightGray);
    }

    void printf(const char* text, ...);
    void print(const char* text, term::Color color  = term::Color::LightGray);
    void put_char(char c, term::Color color  = term::Color::LightGray);

    namespace serial {
        void printf(const char* text, ...);
        void print(const char* text);
        void put_char(char c);
    }
}
