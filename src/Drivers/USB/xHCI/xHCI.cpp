#include "xHCI.hpp"
#include "xHCI_common.hpp"
#include "xHCI_mem.hpp"
#include "std/printf.hpp"
#include "Drivers/PCI.hpp"

namespace USB {
    xhci_driver m_xhci_driver;

    bool xhci_driver::init_device() {
        // Get xHCI device from PCI
        PCI::PCI_Device usb = PCI::Find_Class(0x0030030C);
        irq_number = PCI::pci_read8(usb.bus, usb.device, usb.function, 0x3C);

        // Get base mmio address
        if (usb.vendor_id == 0) {
            std::printf("No xHCI Device found\n");
            return false;
        }

        const uint32_t bar0 = usb.bar[0];
        const uint32_t bar1 = usb.bar[1];

        if (bar0 & 1) {
            std::kernel::printf("xHCI should not use IO BAR");
            return false;
        }
        const uint32_t type = (bar0 >> 1) & 0b11;

        if (type == 0b00) {
            // 32bit addr
            m_xhci_base = bar0 & ~0xFULL;
        } else if (type == 0b10) {
            // 64bit addr
            m_xhci_base = (static_cast<uint64_t>(bar1) << 32) | (bar0 & ~0xFULL);
        }

        xhci_map_mmio(m_xhci_base, 0x10000);

        _parse_capability_registers();
        _log_capability_registers();

        return true;
    }

    bool xhci_driver::start_device() {
        is_running = true;
        return true;
    }

    bool xhci_driver::shutdown_device() {
        is_running = false;
        return true;
    }

    void xhci_driver::_parse_capability_registers() {
        m_cap_regs = reinterpret_cast<volatile xhci_capability_registers *>(m_xhci_base);

        m_capability_regs_length = m_cap_regs->caplength;

        m_max_device_slots = XHCI_MAX_DEVICE_SLOTS(m_cap_regs);
        m_max_interrupters = XHCI_MAX_INTERRUPTERS(m_cap_regs);
        m_max_ports = XHCI_MAX_PORTS(m_cap_regs);

        m_isochronous_scheduling_threshold = XHCI_IST(m_cap_regs);
        m_erst_max = XHCI_ERST_MAX(m_cap_regs);
        m_max_scratchpad_buffers = XHCI_MAX_SCRATCHPAD_BUFFERS(m_cap_regs);

        m_64bit_addressing_capability = XHCI_AC64(m_cap_regs);
        m_bandwidth_negotiation_capability = XHCI_BNC(m_cap_regs);
        m_64byte_context_size = XHCI_CSZ(m_cap_regs);
        m_port_power_control = XHCI_PPC(m_cap_regs);
        m_port_indicators = XHCI_PIND(m_cap_regs);
        m_light_reset_capability = XHCI_LHRC(m_cap_regs);
        m_extended_capabilities_offset = XHCI_XECP(m_cap_regs) * sizeof(uint32_t);
    }

    void xhci_driver::_log_capability_registers() {
        std::kernel::printf("&f===== Xhci Capability Registers (&a%x&f) =====\n", reinterpret_cast<uint64_t>(m_cap_regs));
        std::kernel::printf("&f    Length                : &a%i\n", m_capability_regs_length);
        std::kernel::printf("&f    Max Device Slots      : &a%i\n", m_max_device_slots);
        std::kernel::printf("&f    Max Interrupters      : &a%i\n", m_max_interrupters);
        std::kernel::printf("&f    Max Ports             : &a%i\n", m_max_ports);
        std::kernel::printf("&f    IST                   : &a%i\n", m_isochronous_scheduling_threshold);
        std::kernel::printf("&f    ERST Max Size         : &a%i\n", m_erst_max);
        std::kernel::printf("&f    Scratchpad Buffers    : &a%i\n", m_max_scratchpad_buffers);
        std::kernel::printf("&f    64-bit Addressing     : &e%s\n", m_64bit_addressing_capability ? "yes" : "no");
        std::kernel::printf("&f    Bandwidth Negotiation : &a%i\n", m_bandwidth_negotiation_capability);
        std::kernel::printf("&f    64-byte Context Size  : &e%s\n", m_64byte_context_size ? "yes" : "no");
        std::kernel::printf("&f    Port Power Control    : &a%i\n", m_port_power_control);
        std::kernel::printf("&f    Port Indicators       : &a%i\n", m_port_indicators);
        std::kernel::printf("&f    Light Reset Available : &a%i\n", m_light_reset_capability);
        std::kernel::printf("\n");
    }
}
