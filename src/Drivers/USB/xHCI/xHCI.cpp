#include "xHCI.hpp"

#include <kernel/Sleep.hpp>
#include <kernel/Memory/heap.hpp>

#include "xHCI_common.hpp"
#include "xHCI_mem.hpp"
#include "std/printf.hpp"
#include "Drivers/PCI.hpp"
#include "xHCI_trb.hpp"
#include "xHCI_rings.hpp"

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

        Time::Sleep(100);

        _parse_capability_registers();
        //_log_capability_registers();
        //_log_operational_registers();

        if (!reset_host_controller()) {
            return false;
        }

        _configure_operational_register();
        _log_operational_registers();

        _configure_runtime_registers();

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

        m_op_regs = reinterpret_cast<volatile xhci_operational_registers*>(m_xhci_base + m_capability_regs_length);

        m_runtime_regs = reinterpret_cast<volatile xhci_runtime_registers*>(m_xhci_base + m_cap_regs->rtsoff);
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

    void xhci_driver::_log_operational_registers() {
        std::kernel::printf("&7===== &fXhci Operational Registers (&a%x&f) &7=====\n", reinterpret_cast<uint64_t>(m_op_regs));
        std::kernel::printf("&f    usbcmd     : &a%x\n", m_op_regs->usbcmd);
        std::kernel::printf("&f    usbsts     : &a%x\n", m_op_regs->usbsts);
        std::kernel::printf("&f    pagesize   : &a%x\n", m_op_regs->pagesize);
        std::kernel::printf("&f    dnctrl     : &a%x\n", m_op_regs->dnctrl);
        std::kernel::printf("&f    crcr       : &a%x\n", m_op_regs->crcr);
        std::kernel::printf("&f    dcbaap     : &a%x\n", m_op_regs->dcbaap);
        std::kernel::printf("&f    config     : &a%x\n", m_op_regs->config);
        std::kernel::printf("\n");
    }

    bool xhci_driver::reset_host_controller() {
        // Clear Run/Stop bit
        uint32_t usbcmd = m_op_regs->usbcmd;
        usbcmd &= ~XHCI_USBCMD_RUN_STOP;
        m_op_regs->usbcmd = usbcmd;

        // Wait for HCHalted bit to be set
        uint32_t timeout = 20; // 200ms timeout
        while (!(m_op_regs->usbsts & XHCI_USBSTS_HCH)) {
            if (--timeout <= 0) {
                std::kernel::printf("Host controller did not halt within %ums\n", timeout);
                return false;
            }

            Time::Sleep(10);
        }

        // Set reset bit
        usbcmd = m_op_regs->usbcmd;
        usbcmd |= XHCI_USBCMD_HCRESET;
        m_op_regs->usbcmd = usbcmd;

        // Wait for Reset bit and CNR bit to clear
        timeout = 100; // 1000ms timeout
        while (m_op_regs->usbcmd & XHCI_USBCMD_HCRESET || m_op_regs->usbsts & XHCI_USBSTS_CNR) {
            if (--timeout == 0) {
                std::kernel::printf("Host controller did not reset within %ums\n", timeout);
                return false;
            }

            Time::Sleep(10);
        }

        Time::Sleep(50);

        if (m_op_regs->usbcmd != 0)
            return false;

        if (m_op_regs->dnctrl != 0)
            return false;

        if (m_op_regs->crcr != 0)
            return false;

        if (m_op_regs->dcbaap != 0)
            return false;

        if (m_op_regs->config != 0)
            return false;

        return true;
    }

    void xhci_driver::_configure_operational_register() {
        m_op_regs->dnctrl = 0xffff;

        m_op_regs->config = static_cast<uint32_t>(m_max_device_slots);

        // Setup DCBAA
        _setup_dcbaa();

        m_command_ring = new xhci_command_ring(XHCI_COMMAND_RING_TRB_COUNT);
        m_op_regs->crcr = m_command_ring->get_physical_base() | m_command_ring->get_cycle_bit();
    }

    void xhci_driver::_setup_dcbaa() {
        uint64_t dcbaa_size = sizeof(uintptr_t) * (m_max_device_slots + 1);

        m_dcbaa = static_cast<uint64_t *>(alloc_xhci_memory(dcbaa_size, XHCI_DEVICE_CONTEXT_ALIGNMENT, XHCI_DEVICE_CONTEXT_BOUNDARY));
        m_dcbaa_virtual = static_cast<uint64_t *>(heap::malloc(m_max_device_slots + 1));

        if (m_max_scratchpad_buffers > 0) {
            uint64_t *scrachpad_array = static_cast<uint64_t *>(
                alloc_xhci_memory(m_max_scratchpad_buffers * sizeof(uint64_t), XHCI_DEVICE_CONTEXT_ALIGNMENT, XHCI_DEVICE_CONTEXT_BOUNDARY));

            for (uint8_t i = 0; i < m_max_scratchpad_buffers; i++) {
                void *scrachpad = alloc_xhci_memory(PAGE_SIZE, XHCI_SCRATCHPAD_BUFFERS_ALIGNMENT, XHCI_SCRATCHPAD_BUFFERS_BOUNDARY);
                uint64_t scrachpad_addr = xhci_get_physical_addr(scrachpad);
                scrachpad_array[i] = scrachpad_addr;
            }

            uint64_t scrachpad_array_physical_base = xhci_get_physical_addr(scrachpad_array);
            m_dcbaa[0] = scrachpad_array_physical_base;

            m_dcbaa_virtual[0] = scrachpad_array_physical_base;
        };

        m_op_regs->dcbaap = xhci_get_physical_addr(m_dcbaa);
    }

    void xhci_driver::_configure_runtime_registers() {
        volatile xhci_interrupter_registers* interrupter_regs = &m_runtime_regs->ir[0];

        // Enable interrupts
        uint32_t iman = interrupter_regs->iman;
        iman |= XHCI_IMAN_INTERRUPT_ENABLE;
        interrupter_regs->iman = iman;

        _acknowledge_irq(0);
    }

    void xhci_driver::_acknowledge_irq(uint8_t interrupter) {
        m_op_regs->usbsts = XHCI_USBSTS_EINT;

        volatile xhci_interrupter_registers* interrupter_regs = &m_runtime_regs->ir[interrupter];

        uint32_t iman = interrupter_regs->iman;
        iman |= XHCI_IMAN_INTERRUPT_PENDING;
        interrupter_regs->iman = iman;
    }
}
