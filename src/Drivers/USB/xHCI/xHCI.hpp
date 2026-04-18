#pragma once
#include <std/types.hpp>
#include "xHCI_regs.hpp"

namespace USB {

    class xhci_driver {
    public:
        bool init_device();
        bool start_device();
        bool shutdown_device();

        uintptr_t m_xhci_base;
    private:
        uint8_t irq_number;

        volatile xhci_capability_registers *m_cap_regs;
        volatile xhci_operational_registers *m_op_regs;

        // CAPLENGTH
        uint8_t m_capability_regs_length;

        // HCSPARAMS1
        uint8_t m_max_device_slots;
        uint8_t m_max_interrupters;
        uint8_t m_max_ports;

        // HCSPARAMS2
        uint8_t m_isochronous_scheduling_threshold;
        uint8_t m_erst_max;
        uint8_t m_max_scratchpad_buffers;

        // hccparams1
        bool m_64bit_addressing_capability;
        bool m_bandwidth_negotiation_capability;
        bool m_64byte_context_size;
        bool m_port_power_control;
        bool m_port_indicators;
        bool m_light_reset_capability;
        uint32_t m_extended_capabilities_offset;

        bool is_running = false;

    private:
        void _parse_capability_registers();
        void _log_capability_registers();
    };

    extern xhci_driver m_xhci_driver;
}
