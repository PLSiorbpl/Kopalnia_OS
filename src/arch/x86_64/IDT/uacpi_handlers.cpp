#include "IDT.hpp"

namespace IDT {
    uacpi_interrupt_handler uacpi_handlers[256];
    uacpi_handle uacpi_handlers_ctx[256];

    void install_uacpi_handler(uacpi_interrupt_handler handler, uint8_t irq_no, uacpi_handle ctx) {
        uacpi_handlers[irq_no] = handler;
        uacpi_handlers_ctx[irq_no] = ctx;
    }
}