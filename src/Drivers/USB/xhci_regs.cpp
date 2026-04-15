#include "xhci_regs.hpp"

namespace USB {
    void xhci_doorbell_manager::construct(const uint64_t base) {
        m_doorbell_registers = reinterpret_cast<xhci_doorbell_register *>(base);
    }

    void xhci_doorbell_manager::ring_doorbell(const uint8_t doorbell, const uint8_t target) {
        m_doorbell_registers[doorbell].raw = static_cast<uint32_t>(target);
    }

    void xhci_doorbell_manager::ring_command_doorbell() {
        ring_doorbell(0, 0);
    }

    void xhci_doorbell_manager::ring_controll_endpoint_doorbell(uint8_t doorbell) {
        ring_doorbell(doorbell, 1);
    }
}
