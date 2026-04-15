#pragma once
#include "PLlib/types.hpp"
#include "Drivers/mmio.hpp"
#include "Drivers/PCI.hpp"
#include "xhci_regs.hpp"

namespace USB {
    extern xhci_doorbell_manager m_doorbell_manager;

    extern uint64_t base;
    extern uint64_t size;

    extern PCI::PCI_Device usb;

    struct transfer_request_block {
        uint64_t parameter;
        uint32_t status;
        union {
            struct {
                uint32_t cycle      :1;
                uint32_t rsvd0      :9;
                uint32_t trb_type   :6;
                uint32_t rsvd1      :16;
            };
            uint32_t control;
        };
    };
    static_assert(sizeof(transfer_request_block) == sizeof(uint32_t) * 4);

    struct interrupter_regs {
        uint32_t iman;
        uint32_t imod;
        uint32_t erstsz;
        uint32_t rsvd;
        uint32_t erstba;
        union {
            struct {
                uint64_t segment_index :3;
                uint64_t handler_busy  :1;
                uint64_t dequeue_pointer :60;
            };
            uint64_t erdp;
        };
    };

    struct runtime_registers {
        uint32_t mf_index;
        uint32_t rsvd[7];
        interrupter_regs ir[1024];
    };

    struct xhci_erst_entry {
        uint64_t ring_segment_base_adr;
        uint32_t ring_segment_size;
        uint32_t rsvd;
    } __attribute__((packed));

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

    struct TRB {
        uint32_t data[4];
    };

    struct ERST {
        uint64_t ring_base;
        uint32_t ring_size;
        uint32_t rsvd;
    };

    extern TRB * cmd_ring;

    extern uint32_t Info;
    extern uint32_t Version;
    extern uint32_t caplenght;
    extern uint32_t hcsparams1;
    extern uint32_t hcsparams2;
    extern uint32_t max_ports;
    extern uint32_t max_slots;
    extern uint32_t max_scratchpads;

    extern uint64_t *dcbaa;

    extern uint32_t cmd;

    extern uint8_t irq_no;

    void PreInit();
    void Init();
    void Get_Info();
    bool Restart();
    void Configure_Op_Regs();
    void Setup_DCBAA();
    void Configure_Run_Regs();
    void Acknowladge_irq(uint8_t interrupter);
    bool Start();
    void log_usbsts();

    extern volatile interrupter_regs *m_interrupter_regs;

    extern uint64_t m_segment_trb_count;
    extern transfer_request_block *m_trbs;
    extern uint64_t m_physical_base;
    extern xhci_erst_entry *m_segment_table;
    extern uint64_t m_dequeue_ptr;
    extern uint8_t m_rcs_bit;
    void event_ring(uint64_t max_trbs, volatile interrupter_regs *interrupter);
    void update_erdp();
    bool has_unprocessed_events();
    void dequeue_events();
    void flush_unprocessed_events();
    transfer_request_block *dequeue_trb();
    extern transfer_request_block *trbs_array[64];
    extern uint64_t trb_int;

    extern uint32_t max_trb_count;
    extern uint64_t enqueue_ptr;
    extern transfer_request_block *trbs;
    extern uint64_t physical_base;
    extern uint8_t rcs_bit;

    extern volatile xhci_capability_registers *cap_regs;
    extern volatile runtime_registers* m_runtime_regs;

    void command_ring(uint64_t max_trbs);

    void enqueue(transfer_request_block *trb);

    void Test_Ports();
    void Print_OP();

    void xhci_irq_handler();

    extern MMIO::mmio mio;
}