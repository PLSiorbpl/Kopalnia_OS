#include "IDT.hpp"
#include "PLlib/types.hpp"
#include "PLlib/String_common.hpp"
#include "arch/x86_64/Common/Common.hpp"
#include "Drivers/Keyboard.hpp"
#include "kernel/Sleep.hpp"

namespace IDT {
    struct ISR_Registers {
        // Corrected order to match your pushall macro
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rbp, rbx, rdx, rcx, rax;
        uint64_t int_no, error_code;
        uint64_t rip, cs, rflags, rsp, ss;
    } __attribute__((packed));

    // NOTE do not add [[noreturn]] to this function
    extern "C" void isr_common(const ISR_Registers *regs) {
        if (regs->int_no <= 31) {
            term::print_uint(regs->int_no);
            term::print(" ");
            term::print_uint(regs->error_code);
            term::print(" ");
            // CPU interrupts (bad so we halt cpu)
            while (true)
                asm volatile("hlt");
        }

        // Handlers here
        if (regs->int_no == 32) { // Timer
            Time::tick++;
        }
        if (regs->int_no == 33) { // Keyboard
            uint8_t c = x64::inb(0x60);
            kb::buf.push(c);
        }

        if (regs->int_no >= 32 && regs->int_no <= 47) {
            x64::pic_send_eoi(regs->int_no - 32);
        }
   }
}