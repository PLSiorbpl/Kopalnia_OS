#include "xHCI_regs.hpp"

#include "xHCI_common.hpp"
#include "std/types.hpp"

namespace USB {
    xhci_doorbell_manager::xhci_doorbell_manager(const uint64_t base) {
        m_doorbell_registers = reinterpret_cast<xhci_doorbell_register *>(base);
    }

    void xhci_doorbell_manager::ring_doorbell(const uint8_t doorbell, const uint8_t target) const {
        m_doorbell_registers[doorbell].raw = static_cast<uint32_t>(target);
    }

    void xhci_doorbell_manager::ring_command_doorbell() const {
        ring_doorbell(0, XHCI_DOORBELL_TARGET_COMMAND_RING);
    }

    void xhci_doorbell_manager::ring_control_endpoint_doorbell(const uint8_t doorbell) const {
        ring_doorbell(doorbell, XHCI_DOORBELL_TARGET_CONTROL_EP_RING);
    }
}
