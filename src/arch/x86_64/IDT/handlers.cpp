#include <std/printf.hpp>

#include "IDT.hpp"
#include "PLlib/types.hpp"
#include "PLlib/String_common.hpp"
#include "arch/x86_64/Common/Common.hpp"
#include "Drivers/Keyboard.hpp"
#include "kernel/Sleep.hpp"
#include "Drivers/USB/usb.hpp"

namespace IDT {
    // NOTE do not add [[noreturn]] to this function
    extern "C" void isr_common(const ISR_Registers *regs) {
        if (regs->int_no <= 31) {
            CPU_Errors(regs->int_no, regs->error_code);
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
        if (regs->int_no == 32+USB::irq_no) {
            std::printf("yes");
            USB::xhci_irq_handler();
        }

        if (regs->int_no >= 32 && regs->int_no <= 47) {
            x64::pic_send_eoi(regs->int_no - 32);
        }
    }

    void CPU_Errors(const uint8_t int_no, const uint64_t error_code) {
        term::print(ExceptionName(int_no), term::Color::Red);
        term::print(" ");
        if (int_no > 19) {
            term::print_uint(int_no);
            term::print(" ");
        }
        term::print_uint(error_code);
    }

    const char* ExceptionName(const uint64_t int_no) {
        switch(int_no) {
            case 0: return "Divide Error";
            case 1: return "Debug";
            case 2: return "Non Maskable Interrupt";
            case 3: return "Breakpoint";
            case 4: return "Overflow";
            case 5: return "Bound Range Exceeded";
            case 6: return "Invalid Opcode";
            case 7: return "Device Not Available";
            case 8: return "Double Fault";
            case 10: return "Invalid TSS";
            case 11: return "Segment Not Present";
            case 12: return "Stack-Segment Fault";
            case 13: return "General Protection";
            case 14: return "Page Fault";
            case 16: return "x87 FPU Floating-Point";
            case 17: return "Alignment Check";
            case 18: return "Machine Check";
            case 19: return "SIMD Floating-Point";
            default: return "Unknown";
        }
    }
}