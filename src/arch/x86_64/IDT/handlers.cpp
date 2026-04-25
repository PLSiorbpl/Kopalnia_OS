#include "IDT.hpp"
#include "std/types.hpp"
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

    static bool extended = false;

    isr_t custom_handlers[256] = { nullptr };

    void Install_handler(const isr_t handler, const uint8_t irq_no) {
        if (!handler) {
            std::kernel::printf("Install_handler &cERROR&f: &anull handler\n");
            return;
        }

        if (irq_no >= 16) { // PIC IRQ 0–15
            std::kernel::printf("Install_handler &cERROR&f: &cinvalid &firq &e%u\n", irq_no);
            return;
        }

        const uint8_t vector = irq_no + 32;

        if (custom_handlers[vector]) {
            std::kernel::printf("&cWARNING&f: overwriting handler for &eIRQ &a%u\n", irq_no);
        }

        custom_handlers[vector] = handler;
        std::kernel::printf("Installed irq: #&a%u &7(&fvector: &a%u&7)&f, function addr: &a%x", irq_no, vector, (void*)handler);
    }

    // NOTE do not add [[noreturn]] to this function
    extern "C" void isr_common(const ISR_Registers* regs) {
        if (regs->int_no <= 31) {
            std::kernel::printf("&4%s &c%x\n&4Caused by RIP: &e%x", get_exception_name(regs->int_no), regs->error_code, regs->rip);

            // CPU interrupts (bad so we halt cpu)
            asm volatile("cli; hlt");
        }

        if (custom_handlers[regs->int_no]) {
            custom_handlers[regs->int_no](regs);
        }

        // Handlers here
        if (regs->int_no == 32) { // Timer
            Time::tick++;
        }
        if (regs->int_no == 33) { // Keyboard
            uint8_t raw = x64::inb(0x60);
            if (raw == 0xE0) {
                extended = true;
                x64::pic_send_eoi(1);
                return;
            }

            bool release = (raw & 0x80) != 0;
            uint8_t sc = raw & 0x7F;
            uint8_t code = extended ? sc + 0x59 : sc;
            extended = false;

            if (!release) {
                kb::buf.push(code);
            }
        }
        //if (regs->int_no == 32+USB::irq_no) {
        //    std::printf("yes");
        //    USB::xhci_irq_handler();
        //}

        if (regs->int_no >= 32 && regs->int_no <= 47) {
            x64::pic_send_eoi(regs->int_no - 32);
        }
    }
}
