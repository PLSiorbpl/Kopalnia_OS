#pragma once
#include "ahci_port.h"
#include "std/types.hpp"

namespace drivers::ahci {
    constexpr u64 AHCI_ENABLE_BIT = (1 << 31);

    struct hba_memory {
        volatile u32 capability; // 0x00, Host capability

        volatile u32 ghc; // 0x04, Global host control
        volatile u32 is;	// 0x08, Interrupt status
        volatile u32 pi;	// 0x0C, Port implemented
        volatile u32 vs;	// 0x10, Version
        volatile u32 ccc_ctl; // 0x14, Command completion coalescing control
        volatile u32 ccc_pts; // 0x18, Command completion coalescing ports
        volatile u32 em_loc;	// 0x1C, Enclosure management location
        volatile u32 em_ctl;	// 0x20, Enclosure management control
        volatile u32 cap2; // 0x24, Host capabilities extended
        volatile u32 bohc; // 0x28, BIOS/OS handoff control and status

        // 0x2C - 0x9F, Reserved
        volatile u8 rsv[0xA0-0x2C];

        // 0xA0 - 0xFF, Vendor specific registers
        volatile u8 vendor[0x100-0xA0];

        // 0x100 - 0x10FF, Port control registers
        volatile hba_port ports[32];	// 1 ~ 32
    };

    class ahci {
    public:
        ahci();
        ~ahci();

        void init();
    private:
        void probe_ports();

        ahci_port ports[32] {};
        u8 port_count = 0;
        volatile hba_memory* hba = nullptr;
    };
}
