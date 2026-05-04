#include "IDT.hpp"

namespace IDT {
    uacpi_interrupt_handler uacpi_handlers[256];
    uacpi_handle uacpi_handlers_ctx[256];

    void install_uacpi_handler(uacpi_interrupt_handler handler, uint8_t vector, uacpi_handle ctx) {
        uacpi_handlers[vector] = handler;
        uacpi_handlers_ctx[vector] = ctx;
    }
}