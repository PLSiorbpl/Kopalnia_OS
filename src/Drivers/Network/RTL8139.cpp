#include "RTL8139.hpp"
#include "Drivers/PCI.hpp"
#include "kernel/log.h"
#include "arch/x86_64/Common/Common.hpp"
#include "kernel/Sleep.hpp"
#include "kernel/Memory/heap.hpp"
#include "kernel/Memory/mem_helper.h"
#include "std/mem_common.hpp"

namespace RTL8139 {
    uint16_t io_base = 0;
    uint8_t mac[6] = {};
    uint16_t rx_offset = 0;

    uint8_t *rx_buffer;

    uint8_t tx_slot = 0;
    uint8_t *tx_buffers[4];
    uint8_t *tx_base;

    bool init() {
        auto device = PCI::find_vendor_class(0x10EC, 0x02, 0x00);
        if (device.vendor_id == 0) {
            log::error("[ NET ] RTL8139 not found");
            return false;
        }

        if (device.device_id != 0x8139) {
            log::error("[ NET ] Found Realtek but not RTL8139: %x", device.device_id);
            return false;
        }

        log::success("[ NET ] RTL8139 found");

        uint16_t cmd = PCI::pci_read16(device.bus, device.device, device.function, 0x04);
        cmd |= (1 << 2) | (1 << 0); // bus master, IO space
        PCI::pci_write16(device.bus, device.device, device.function, 0x04, cmd);

        io_base = device.bar[0] & 0xFFFC;

        // Power on
        x64::outb(io_base + REG_CONFIG1, 0x0);

        // Software reset
        x64::outb(io_base + REG_CMD, 0x10);
        int timeout = 5;
        while( (x64::inb(io_base + REG_CMD) & 0x10) != 0) {
            if (timeout-- <= 0) {
                log::error("[ NET ] RTL8139 Timed out");
                return false;
            }
            Time::Sleep(10);
        }

        // Read MAC
        for (int i = 0; i < 6; i++)
            mac[i] = x64::inb(io_base + i);

        log::success(
        "[ NET ] RTL8139 MAC: %x:%x:%x:%x:%x:%x",
        mac[0], mac[1], mac[2],
        mac[3], mac[4], mac[5]);

        // RX buffer
        rx_buffer = static_cast<uint8_t *>(heap::malloc_align(RX_BUF_SIZE, 4096));
        x64::outl(io_base + REG_RBSTART, to_physical(rx_buffer));
        log::info("%x, %x", rx_buffer, to_physical(rx_buffer));

        // RX config
        x64::outl(io_base + REG_RCR, RCR_VAL);

        // enable interrupts
        x64::outw(io_base + REG_IMR, 0x0005);

        // Clear pending interrupts
        x64::outw(io_base + REG_ISR, 0xFFFF);

        // Enable RX TX
        x64::outb(io_base + REG_CMD, 0x0C);

        // Reset CARP
        rx_offset = 0;
        x64::outw(io_base + REG_CAPR, 0xFFF0);

        // Setup TX buffers
        tx_base = static_cast<uint8_t *>(heap::malloc_align(2048 * 4, 4096));
        tx_buffers[0] = tx_base;
        tx_buffers[1] = tx_base + 2048;
        tx_buffers[2] = tx_base + 4096;
        tx_buffers[3] = tx_base + 6144;

        PCI::install_interrupt(device, handle_rx, 15);

        log::success("[ NET ] RTL8139 initialized\n");
        return true;
    }

    void handle_rx(const IDT::ISR_Registers *regs) {
        uint16_t isr = x64::inw(io_base + REG_ISR);
        x64::outw(io_base + REG_ISR, isr);

        while ((x64::inb(io_base + REG_CMD) & 0x01) == 0) {
            // Header: 2B status, 2B length
            const auto* hdr    = reinterpret_cast<uint16_t*>(rx_buffer + rx_offset);
            uint16_t  status = hdr[0];
            uint16_t  length = hdr[1];

            if (!(status & 0x1)) {
                log::info("ROK not set");
                break;
            }
            if (status & (1 << 1)) {
                log::info("Frame alignment error");
            }
            if (status & (1 << 2)) {
                log::info("CRC error");
            }
            if (status & (1 << 3)) {
                log::info("Packet too long");
            }
            if (status & (1 << 4)) {
                log::info("Runt packet");
            }

            uint8_t* packet = rx_buffer + rx_offset + 4;

            // Parsing:
            uint8_t dst_mac[6];
            uint8_t src_mac[6];
            for (int i = 0; i < 6; i++) {
                dst_mac[i] = packet[i];
                src_mac[i] = packet[i + 6];
            }
            uint16_t ethertype = (packet[12] << 8) | packet[13];

            // Protocols
            if (ethertype == 0x0806) {
                log::info("ARP");
                ARPPacket reply{};

                mem::memcpy(reply.eth.dst_mac, src_mac, 6);

                mem::memcpy(reply.eth.src_mac, mac, 6);

                reply.eth.ethertype = htons(0x0806);

                reply.arp.hardware_type = htons(1);

                reply.arp.protocol_type = htons(0x0800);

                reply.arp.mac_len = 6;
                reply.arp.ip_len = 4;

                reply.arp.opcode = htons(2); // Reply

                mem::memcpy(reply.arp.sender_mac, mac, 6);
                reply.arp.sender_ip = htonl(0x0A000002); // 10.0.0.2

                mem::memcpy(reply.arp.target_mac, src_mac, 6);
                reply.arp.target_ip = *reinterpret_cast<uint32_t *>(&packet[28]);

                send(reinterpret_cast<uint8_t *>(&reply), sizeof(ARPPacket));
            }

            log::info("EtherType: %x", ethertype);

            // Advance and align to 4B
            rx_offset = (rx_offset + length + 4 + 3) & ~3;
            rx_offset %= (8192 + 16);

            x64::outw(io_base + REG_CAPR, rx_offset - 16);
        }
    }

    bool send(const uint8_t* data, uint16_t length) {
        if (length > 1500) {
            log::error("[ NET ] Packet too large: %u", length);
            return false;
        }

        mem::memcpy(tx_buffers[tx_slot], data, length);

        const auto phys_tx = to_physical(tx_buffers[tx_slot]);

        // Set TX address and length
        x64::outl(io_base + REG_TSAD0 + tx_slot * 4, static_cast<uint32_t>(phys_tx));
        x64::outl(io_base + REG_TSD0  + tx_slot * 4, length & 0x1FFF);

        tx_slot = (tx_slot + 1) % 4;
        return true;
    }
}
