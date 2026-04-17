#include "syscall.h"
#include "String_common.hpp"
#include "arch/x86_64/Common/Common.hpp"

extern "C" u64 kernel_rsp = 0;
extern "C" u64 user_rsp = 0;

auto validate_user_ptr = [](const u64 ptr) -> bool {
    return ptr != 0 && ptr < 0x800000000000ULL;
};

enum class syscall : u64 {
    write = 0,
    put_char = 1,
    serial_write = 2,
    serial_put_char = 3,
    exit = 4,
};

extern "C" u64 dispatch_syscall(u64 id, u64 arg1, u64 arg2, u64 arg3) {
    switch (static_cast<syscall>(id)) {
        case syscall::write:
            if (!validate_user_ptr(arg1))
                return static_cast<u64>(-1);
            term::print(reinterpret_cast<const char*>(arg1), static_cast<term::Color>(arg2));
            return 0;
        case syscall::put_char:
            term::put_char(static_cast<char>(arg1), static_cast<term::Color>(arg2));
            return 0;
        case syscall::serial_put_char:
            while (!(x64::inb(0x3F8 + 5) & 0x20)) { }
            x64::outb(0x3F8, static_cast<char>(arg1));
            return 0;
        case syscall::serial_write: {
            if (!validate_user_ptr(arg1))
                return static_cast<u64>(-1);
            const auto text = reinterpret_cast<const char *>(arg1);
            for (int i = 0; text[i] != '\0'; i++) {
                while (!(x64::inb(0x3F8 + 5) & 0x20)) { }
                x64::outb(0x3F8, text[i]);
            }
            return 0;
        }
        case syscall::exit:
            return 0;
        default:
            return static_cast<u64>(-1); // ENOSYS
    }
}