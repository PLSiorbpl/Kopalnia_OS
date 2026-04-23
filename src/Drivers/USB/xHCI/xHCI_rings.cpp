#include "xHCI_rings.hpp"

#include "xHCI_common.hpp"
#include "xHCI_mem.hpp"
#include "std/types.hpp"

namespace USB {
    xhci_command_ring::xhci_command_ring(size_t max_trbs) {
        m_max_trb_count = max_trbs;
        m_rcs_bit = 1;
        m_enqueue_ptr = 0;

        const uint64_t ring_size = max_trbs * sizeof(xhci_trb_t);
        m_trbs = static_cast<xhci_trb_t *>(alloc_xhci_memory(ring_size, XHCI_COMMAND_RING_SEGMENTS_ALIGNMENT,
                                                             XHCI_COMMAND_RING_SEGMENTS_BOUNDARY));

        m_physical_base = xhci_get_physical_addr(m_trbs);

        m_trbs[max_trbs - 1].parameter = m_physical_base;
        m_trbs[max_trbs - 1].control = (XHCI_TRB_TYPE_LINK << XHCI_TRB_TYPE_SHIFT) | XHCI_LINK_TRB_TC_BIT | m_rcs_bit;
    }

    void xhci_command_ring::enqueue(xhci_trb_t *trb) {
        trb->cycle_bit = m_rcs_bit;
        m_trbs[m_enqueue_ptr] = *trb;

        if (++m_enqueue_ptr == m_max_trb_count - 1) {
            m_trbs[m_max_trb_count - 1].control = (XHCI_TRB_TYPE_LINK << XHCI_TRB_TYPE_SHIFT) | XHCI_LINK_TRB_TC_BIT | m_rcs_bit;
            m_enqueue_ptr = 0;
            m_rcs_bit = !m_rcs_bit;
        }
    }
}
