#include "usb.hpp"

#include <mem_common.hpp>
#include <arch/x86_64/Common/Common.hpp>
#include <kernel/Paging.hpp>
#include <kernel/Memory/heap.hpp>

#include "Drivers/PCI.hpp"
#include "PLlib/types.hpp"
#include "std/printf.hpp"
#include "Drivers/mmio.hpp"
#include "kernel/Sleep.hpp"
#include "std/vector.hpp"

namespace USB {
    xhci_doorbell_manager m_doorbell_manager;

    uint64_t base;
    uint64_t size;

    MMIO::mmio mio;

    PCI::PCI_Device usb;
    TRB *cmd_ring;

    uint8_t irq_no;

    void PreInit() {
        usb = PCI::Find_Class(0x0030030C);
        irq_no = PCI::pci_read8(usb.bus, usb.device, usb.function, 0x3C);
        std::printf("irq_no = %u", irq_no);
        if (usb.vendor_id == 0) {
            std::printf("No USB Device found\n");
            return;
        }

        const uint32_t bar0 = usb.bar[0];
        const uint32_t bar1 = usb.bar[1];

        if (bar0 & 1) {

        } else {
            uint32_t type = (bar0 >> 1) & 0b11;

            if (type == 0b00) {
                std::printf("32-bit MMIO BAR NOT SUPPORTED\n");
            } else if (type == 0b10) {
                base = (static_cast<uint64_t>(bar1) << 32) | (bar0 & ~0xFULL);
                size = 0x10000;
                mio.base = base;
            }
        }
    }

    uint32_t Info;
    uint32_t Version;
    uint32_t caplenght;
    uint32_t hcsparams1;
    uint32_t hcsparams2;
    uint32_t max_ports;
    uint32_t max_slots;
    uint32_t max_scratchpads;

    volatile xhci_capability_registers *cap_regs;
    volatile runtime_registers* m_runtime_regs;

    uint64_t *dcbaa;

    uint32_t cmd;

    uint32_t max_trb_count;
    uint64_t enqueue_ptr;
    transfer_request_block *trbs;
    uint64_t physical_base;
    uint8_t rcs_bit;

    volatile interrupter_regs *m_interrupter_regs;
    transfer_request_block *trbs_array[64];
    uint64_t trb_int = 0;
    uint64_t m_segment_trb_count;
    transfer_request_block *m_trbs;
    uint64_t m_physical_base;
    xhci_erst_entry *m_segment_table;
    uint64_t m_dequeue_ptr;
    uint8_t m_rcs_bit;

    // Capability Registers
    #define CAPLENGTH 0x00
    #define RSVD 0x01
    #define HCIVERSION 0x02
    #define HCSPARAMS1 0x04
    #define HCSPARAMS2 0x08
    #define HCSPARAMS3 0xOC
    #define HCCPARAMS1 0x10
    #define DBOFF 0x14
    #define RTSOFF 0x18
    #define HCCPARMS2 0x1C

    // Operational Registers
    #define USBCMD 0x00
    #define USBSTS 0x04
    #define DNCTRL 0x14
    #define CRCR 0x18
    #define DCBAAP 0x30
    #define CONFIG 0x38

    // Runtime Registers
    #define IMAN 0x00
    #define IMOD 0x04
    #define ERSTSZ 0x8
    #define ERSTBA 0x10
    #define ERDP 0x18

    void Print_OP() {
        std::printf("USBCMD: %x\n", mio.MMIO_READ32(caplenght + USBCMD));
        std::printf("USBSTS: %x\n", mio.MMIO_READ32(caplenght + USBSTS));
        std::printf("DNCTRL: %x\n", mio.MMIO_READ32(caplenght + DNCTRL));
        std::printf("CRCR: %x\n", mio.MMIO_READ32(caplenght + CRCR));
        std::printf("DCBAAP: %x\n", mio.MMIO_READ32(caplenght + DCBAAP));
        std::printf("CONFIG: %x\n", mio.MMIO_READ32(caplenght + CONFIG));
    }

    void Get_Info() {
        cap_regs = reinterpret_cast<volatile xhci_capability_registers*>(base);
        Info = mio.MMIO_READ32(CAPLENGTH);
        Version = (Info >> 16)&0xFFFF;
        caplenght = Info&0xFF;

        hcsparams1 = mio.MMIO_READ32(HCSPARAMS1);
        max_slots = hcsparams1 & 0xFF;
        max_ports = (hcsparams1 >> 24) & 0xFF;

        hcsparams2 = mio.MMIO_READ32(HCSPARAMS2);
        const uint32_t hi = (hcsparams2 >> 21) & 0x1F;
        const uint32_t lo = (hcsparams2 >> 27) & 0x1F;

        max_scratchpads = (hi << 5) | lo;

        m_runtime_regs = reinterpret_cast<runtime_registers*>(base + (cap_regs->rtsoff & ~0x1F));

        std::printf("USB INFO: Ver: %u Max Slots: %u Max Ports: %u\n", Version, max_slots, max_ports);

        m_doorbell_manager.construct(base + (cap_regs->dboff & ~0x3));
    }

    bool Restart() {
        // Halt Controller
        cmd = mio.MMIO_READ32(USBCMD + caplenght);
        cmd &= ~1;
        mio.MMIO_WRITE32(USBCMD + caplenght, cmd);

        while (!(mio.MMIO_READ32(USBSTS + caplenght) & 1)) {
            x64::halt();
        }
        Time::Sleep(50); // 50ms additionally
        std::printf("Controller halted -> ");

        // Reset Controller
        cmd = mio.MMIO_READ32(USBCMD + caplenght);
        cmd |= (1 << 1);
        mio.MMIO_WRITE32(USBCMD + caplenght, cmd);

        while (mio.MMIO_READ32(USBCMD + caplenght) & (1 << 1) && mio.MMIO_READ32(USBSTS + caplenght) & (1 << 11)) {
            x64::halt();
        }
        Time::Sleep(100); // Aditional 100ms

        // Check Controller
        if (mio.MMIO_READ32(USBCMD + caplenght) != 0)
            return false;
        if (mio.MMIO_READ32(DNCTRL + caplenght) != 0)
            return false;
        if (mio.MMIO_READ32(CRCR + caplenght) != 0)
            return false;
        if (mio.MMIO_READ32(DCBAAP + caplenght) != 0)
            return false;
        if (mio.MMIO_READ32(CONFIG + caplenght) != 0)
            return false;

        std::printf("Controller Restarted -> ");
        return true;
    }

    void Configure_Op_Regs() {
        // Enable Device Notifications
        mio.MMIO_WRITE32(DNCTRL + caplenght, 0xFFFF);

        mio.MMIO_WRITE32(CONFIG + caplenght, max_slots);

        //DCBAA
        Setup_DCBAA();

        // Setup Command ring and write crcr
        command_ring(256);
        uint64_t crcr12 = physical_base;
        crcr12 &= ~0x3FULL;
        crcr12 |= rcs_bit;
        mio.MMIO_WRITE64(CRCR + caplenght, crcr12);

        //Print_OP();

        Configure_Run_Regs();
    }

    void Setup_DCBAA() {
        const uint64_t dcbaa_size = sizeof(uint64_t) * (max_slots + 1);

        dcbaa = static_cast<uint64_t *>(heap::malloc_aligned(dcbaa_size, 64, 0));
        mem::memset(dcbaa, 0, dcbaa_size);
        if (max_scratchpads > 0) {
            auto *scrachpad_array = static_cast<uint64_t *>(heap::malloc_aligned(max_scratchpads * sizeof(uint64_t), 64, 0));
            mem::memset(scrachpad_array, 0, max_scratchpads * sizeof(uint64_t));

            for (uint32_t i = 0; i < max_scratchpads; i++) {
                auto *scrachpad = static_cast<uint64_t *>(heap::malloc_aligned(4096, 4096, 0));
                scrachpad_array[i] = reinterpret_cast<uint64_t>(scrachpad);
            }

            dcbaa[0] = reinterpret_cast<uint64_t>(scrachpad_array);
        }

        mio.MMIO_WRITE64(DCBAAP + caplenght, reinterpret_cast<uint64_t>(dcbaa));
    }

    void command_ring(uint64_t max_trbs) {
        max_trb_count = max_trbs;
        rcs_bit = 1;
        enqueue_ptr = 0;

        const uint64_t ring_size = max_trb_count * sizeof(transfer_request_block);
        trbs = static_cast<transfer_request_block *>(heap::malloc_aligned(ring_size, 64, 1024 * 64));

        physical_base = reinterpret_cast<uint64_t>(trbs);

        trbs[max_trb_count-1].parameter = physical_base;
        trbs[max_trb_count-1].control = (6 << 10) | (1 << 1) | rcs_bit;
    }

    void enqueue(transfer_request_block *trb) {
        trb->cycle = rcs_bit;
        trbs[enqueue_ptr] = *trb;

        if (++enqueue_ptr == max_trb_count - 1) {
            trbs[max_trb_count-1].control = (6 << 10) | (1 << 1) | rcs_bit;

            enqueue_ptr = 0;
            rcs_bit = !rcs_bit;
        }
    }

    void Configure_Run_Regs() {
        volatile interrupter_regs *int_regs = &m_runtime_regs->ir[0];

        uint32_t iman = int_regs->iman;
        iman |= (1 << 1);
        int_regs->iman = iman;

        event_ring(256, int_regs);

        //std::printf("erstsz: %x\n", int_regs->erstsz);
        //std::printf("erstba: %x\n", int_regs->erstba);
        //std::printf("erdp: %x\n", int_regs->erdp);

        Acknowladge_irq(0);
    }

    void Acknowladge_irq(uint8_t interrupter) {
        mio.MMIO_WRITE32(USBSTS + caplenght, (1 << 3));

        volatile interrupter_regs *int_regs = &m_runtime_regs->ir[interrupter];

        uint32_t iman = int_regs->iman;

        iman |= (1 << 0);

        int_regs->iman = iman;
    }

    void event_ring(uint64_t max_trbs, volatile interrupter_regs *interrupter) {
        m_interrupter_regs = interrupter;
        m_segment_trb_count = max_trbs;
        m_rcs_bit = (1 << 0);

        constexpr uint64_t segment_count = 1;
        const uint64_t segment_size = max_trbs * sizeof(transfer_request_block);
        const uint64_t segment_table_size = segment_count * sizeof(xhci_erst_entry);

        m_trbs = static_cast<transfer_request_block *>(heap::malloc_aligned(segment_size, 64, 1024 * 64));
        m_physical_base = reinterpret_cast<uint64_t>(m_trbs);

        m_segment_table = static_cast<xhci_erst_entry *>(heap::malloc_aligned(segment_table_size, 64, 0x1000));

        xhci_erst_entry entry;
        entry.ring_segment_base_adr = m_physical_base;
        entry.ring_segment_size = m_segment_trb_count;
        entry.rsvd = 0;

        m_segment_table[0] = entry;
        m_interrupter_regs->erstsz = 1;
        m_interrupter_regs->erstba = reinterpret_cast<uint64_t>(m_segment_table);
        update_erdp();
    }

    void update_erdp() {
        uint64_t dequeue_adrress = m_physical_base + (m_dequeue_ptr * sizeof(transfer_request_block));
        m_interrupter_regs->erdp = dequeue_adrress;
    }

    transfer_request_block *dequeue_trb() {
        if (m_trbs[m_dequeue_ptr].cycle != m_rcs_bit) {
            std::printf("Event ring attempted dequeue invalid TRB returning nullptr\n");
            return nullptr;
        }
        transfer_request_block *ret = &m_trbs[m_dequeue_ptr];

        if (++m_dequeue_ptr == m_segment_trb_count) {
            m_dequeue_ptr = 0;
            m_rcs_bit = !m_rcs_bit;
        }

        return ret;
    }

    bool has_unprocessed_events() {
        return (m_trbs[m_dequeue_ptr].cycle == m_rcs_bit);
    }

    void dequeue_events() {
        while (has_unprocessed_events()) {
            transfer_request_block *trb = dequeue_trb();
            if (!trb) {
                break;
            }

            trbs_array[trb_int] = trb;
            trb_int++;

            uint64_t erdp = m_interrupter_regs->erdp;
            erdp |= (1 << 3);
            m_interrupter_regs->erdp = erdp;
        }
    }

    void flush_unprocessed_events() {
        while (has_unprocessed_events()) {
            transfer_request_block *trb = dequeue_trb();
            if (!trb) {
                break;
            }

            uint64_t erdp = m_interrupter_regs->erdp;
            erdp |= (1 << 3);
            m_interrupter_regs->erdp = erdp;
        }
    }

    bool Start() {
        uint32_t usbcmd = mio.MMIO_READ32(USBCMD + caplenght);
        usbcmd |= (1 << 0);
        usbcmd |= (1 << 2);
        mio.MMIO_WRITE32(USBCMD + caplenght, usbcmd);

        while (mio.MMIO_READ32(USBSTS + caplenght) & (1 << 0)) {
            x64::halt();
        }
        Time::Sleep(200);

        if (mio.MMIO_READ32(USBSTS + caplenght) & (1 << 11))
            return false;

        transfer_request_block trb{0};
        trb.trb_type = 9;

        enqueue(&trb);

        m_doorbell_manager.ring_command_doorbell();

        return true;
    }

    void log_usbsts() {
        uint32_t status = mio.MMIO_READ32(USBSTS + caplenght);
        std::printf("===== USBSTS =====\n");
        if (status & (1 << 0))  std::printf("    Host Controlled Halted\n");
        if (status & (1 << 2))  std::printf("    Host System Error\n");
        if (status & (1 << 3)) std::printf("    Event Interrupt\n");
        if (status & (1 << 4))  std::printf("    Port Change Detect\n");
        if (status & (1 << 8))  std::printf("    Save State Status\n");
        if (status & (1 << 9))  std::printf("    Restore State Status\n");
        if (status & (1 << 10))  std::printf("    Save/Restore Error\n");
        if (status & (1 << 11))  std::printf("    Controller Not Ready\n");
        if (status & (1 << 12))  std::printf("    Host Controller Error\n");
        std::printf("\n");
    }

    void xhci_irq_handler() {
        if (has_unprocessed_events()) {
            dequeue_events();
        }

        for (int i = 0; i < 10; i++) {
            std::printf("event: %u", trbs_array[i]);
        }

        Acknowladge_irq(0);
    }

    void Init() {
        if (usb.vendor_id == 0) {
            return;
        }
        Get_Info();

        // Restart
        if (!Restart()) {
            std::printf("Controller Failed to reset");
        }

        // Config
        Configure_Op_Regs();
        std::printf("Controller Started\n");
        std::printf("usbsts before: %x\n", mio.MMIO_READ32(USBSTS + caplenght));
        if (!Start()) {
            std::printf("Failed to start controller\n");
            return;
        }
        std::printf("usbsts after: %x\n", mio.MMIO_READ32(USBSTS + caplenght));

        log_usbsts();
    }

    void Test_Ports() {
        for (uint32_t i = 0; i < max_ports; i++) {
            const uint32_t port = 0x400 + caplenght + i*0x10;
            uint32_t portsc = mio.MMIO_READ32(port);
            if ((portsc & 1) && (portsc & (1 << 1))) {
                std::printf("Active Device at Port: %u = %x\n", i, portsc);
            }
        }
        log_usbsts();
    }
}
