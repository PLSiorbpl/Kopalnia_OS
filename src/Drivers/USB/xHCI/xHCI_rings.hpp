#pragma once
#include "xHCI_trb.hpp"
#include "std/types.hpp"

namespace USB {
    class xhci_command_ring {
    public:
        xhci_command_ring(size_t max_trbs);

        xhci_trb_t* get_virtual_base() const { return m_trbs; }
        uintptr_t get_physical_base() const { return m_physical_base; }
        uint8_t  get_cycle_bit() const { return m_rcs_bit; }

        void enqueue(xhci_trb_t *trb);

    private:
        size_t m_max_trb_count;
        size_t m_enqueue_ptr;
        xhci_trb_t* m_trbs;
        uintptr_t m_physical_base;
        uint8_t m_rcs_bit;
    };
}
