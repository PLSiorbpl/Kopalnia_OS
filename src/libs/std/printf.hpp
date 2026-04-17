#pragma once
#include "Drivers/types.h"

namespace std {
    enum class Output {
        std_out = 0,
        std_serial = 1,
    };

    namespace kernel {
        void printf(const char* text, ...);
        void print(const char* text, Color color = Color::LightGray);
        void put_char(char c, Color color = Color::LightGray);
    }

    void printf(const char* text, Output out = Output::std_out, ...);
    void print(const char* text, Output out = Output::std_out, Color color = Color::LightGray);
    void put_char(char c, Output out = Output::std_out, Color color = Color::LightGray);
}
