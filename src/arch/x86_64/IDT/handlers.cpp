#include "IDT.hpp"
#include "PLlib/types.hpp"
#include "PLlib/String_common.hpp"

namespace IDT {
    struct ISR_Registers {
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
        uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
        uint64_t vector_number;
        uint64_t error_code;
    };

    extern "C" void isr_common(const ISR_Registers *regs) {
        term::print_uint(regs->vector_number);
        term::print(" ");
        term::print_uint(regs->error_code);
        term::print(" ");
   }
}