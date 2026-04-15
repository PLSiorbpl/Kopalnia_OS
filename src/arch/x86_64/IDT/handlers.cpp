#include "IDT.hpp"
#include "../../../libs/std/types.hpp"
#include "libs/String_common.hpp"
#include "arch/x86_64/Common/Common.hpp"
#include "Drivers/Keyboard.hpp"
#include "kernel/Sleep.hpp"
#include "std/printf.hpp"

namespace IDT {
    const char* get_exception_name(const uint64_t int_no) {
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
            case 20: return "Virtualization Exception";
            case 21: return "Control Protection Exception (yes its acronym is #CP)";
            case 28: return "Hypervisor Injection Exception";
            case 29: return "VMM Communication Exception";
            case 30: return "Security Exception";
            default: return "Unknown";
        }
    }

    // NOTE do not add [[noreturn]] to this function
    extern "C" void isr_common(const ISR_Registers* regs) {
        if (regs->int_no <= 31) {
            std::printf("&4%s &c%i\n&4Caused by line: &e%x", get_exception_name(regs->int_no), regs->error_code, *reinterpret_cast<uint8_t*>(regs->rip));

            // CPU interrupts (bad so we halt cpu)
            asm volatile("cli; hlt");
        }

        // Handlers here
        if (regs->int_no == 32) { // Timer
            Time::tick++;
        }
        if (regs->int_no == 33) { // Keyboard
            uint8_t c = x64::inb(0x60);
            kb::buf.push(c);
        }
        // if (regs->int_no == 32+USB::irq_no) {
        //     std::printf("yes");
        //     USB::xhci_irq_handler();
        // }

        if (regs->int_no >= 32 && regs->int_no <= 47) {
            x64::pic_send_eoi(regs->int_no - 32);
        }
    }
}
