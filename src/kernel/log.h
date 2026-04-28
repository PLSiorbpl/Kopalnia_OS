#pragma once
#include "../libs/std/printf.hpp"
#include "std/string.h"

namespace log {
    constexpr u32 MAX_LOG_LEVEL = 0; // 0 = error, 1 = warnings and above, 2 = success and above, 3 = info and above

    template<typename... Args>
    void error(const char* text, Args&&... args) {
        if constexpr (MAX_LOG_LEVEL < 0)
            return;

        char buf[1024];
        int n = 0;
        n += std::strcpy(buf + n, "&4[KERNEL ERROR] ");
        n += std::strcpy(buf + n , text);
        n += std::strcpy(buf + n, "\n");

        std::kernel::printf(buf, args...);
    }

    template<typename... Args>
    void warn(const char* text, Args&&... args) {
        if constexpr (MAX_LOG_LEVEL < 1)
            return;

        char buf[1024];
        int n = 0;
        n += std::strcpy(buf + n, "&e[KERNEL WARN] ");
        n += std::strcpy(buf + n , text);
        n += std::strcpy(buf + n, "\n");

        std::kernel::printf(buf, args...);
    }

    template<typename... Args>
    void info(const char* text, Args&&... args) {
        if constexpr (MAX_LOG_LEVEL < 3)
            return;

        char buf[1024];
        int n = 0;
        n += std::strcpy(buf + n, "&7[KERNEL INFO] ");
        n += std::strcpy(buf + n , text);
        n += std::strcpy(buf + n, "\n");

        std::kernel::printf(buf, args...);
    }

    template<typename... Args>
    void success(const char* text, Args&&... args) {
        if constexpr (MAX_LOG_LEVEL < 2)
            return;

        char buf[1024];
        int n = 0;
        n += std::strcpy(buf + n, "&a[KERNEL SUCCESS] ");
        n += std::strcpy(buf + n , text);
        n += std::strcpy(buf + n, "\n");

        std::kernel::printf(buf, args...);
    }
}
