#pragma once
#include "std/types.hpp"

namespace USB {
    struct xhci_capability_registers {
        const uint8_t caplength;    // Capability Register Length
        const uint8_t reserved0;
        const uint16_t hciversion;  // Interface Version Number
        const uint32_t hcsparams1;  // Structural Parameters 1
        const uint32_t hcsparams2;  // Structural Parameters 2
        const uint32_t hcsparams3;  // Structural Parameters 3
        const uint32_t hccparams1;  // Capability Parameters 1
        const uint32_t dboff;       // Doorbell Offset
        const uint32_t rtsoff;      // Runtime Register Space Offset
        const uint32_t hccparams2;  // Capability Parameters 2
    };
    static_assert(sizeof(xhci_capability_registers) == 32);

    struct xhci_operational_registers {
        uint32_t usbcmd;        // USB Command
        uint32_t usbsts;        // USB Status
        uint32_t pagesize;      // Page Size
        uint32_t reserved0[2];
        uint32_t dnctrl;        // Device Notification Control
        uint64_t crcr;          // Command Ring Control
        uint32_t reserved1[4];
        uint64_t dcbaap;        // Device Context Base Address Array Pointer
        uint32_t config;        // Configure
        uint32_t reserved2[49];
        // Port Register Set offset has to be calculated dynamically based on MAXPORTS
    };
    static_assert(sizeof(xhci_operational_registers) == 256);

    struct xhci_interrupter_registers {
        uint32_t iman;
        uint32_t imod;
        uint32_t erstsz;
        uint32_t rsvd;
        uint64_t erstba;
        union {
            struct {
                uint64_t dequeue_erst_segment_index : 3;
                uint64_t event_handler_busy : 1;
                uint64_t event_ring_dequeue_pointer : 60;
            };
            uint64_t erdp;
        };
    };

    struct xhci_runtime_registers {
        uint32_t mf_index;
        uint32_t rsvdz[7];
        xhci_interrupter_registers ir[1024];
    };

    struct xhci_doorbell_register {
    union {
        struct {
            uint8_t db_target;
            uint8_t rsvd;
            uint16_t db_stream_id;
        };
        uint32_t raw;
    };
    };

    class xhci_doorbell_manager {
    public:
        xhci_doorbell_manager(uint64_t base);

        void ring_doorbell(uint8_t doorbell, uint8_t target) const;

        void ring_command_doorbell() const;
        void ring_control_endpoint_doorbell(uint8_t doorbell) const;

    private:
        xhci_doorbell_register *m_doorbell_registers;

    };
}