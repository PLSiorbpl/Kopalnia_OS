#pragma once
#include "std/types.hpp"

inline u64 sys_write(const char* str, u64 color) {
    u64 ret;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(0ULL), "D"(str), "S"(color)
        : "rcx", "r11", "memory");

    return ret;
}

inline u64 sys_put_char(char c, u64 color) {
    u64 ret;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(1ULL), "D"(static_cast<u64>(c)), "S"(color)
        : "rcx", "r11", "memory");

    return ret;
}

inline u64 sys_serial_write(const char* c) {
    u64 ret;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(2ULL), "D"(c)
        : "rcx", "r11", "memory");

    return ret;
}

inline u64 sys_serial_put_char(char c) {
    u64 ret;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(3ULL), "D"(static_cast<u64>(c))
        : "rcx", "r11", "memory");

    return ret;
}

inline char sys_get_char() {
    u64 ret;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(4ULL)
        : "rcx", "r11", "memory");

    return static_cast<char>(ret);
}