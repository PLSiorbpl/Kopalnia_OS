#pragma once
#include "std/types.hpp"
#include "arch/x86_64/IDT/IDT.hpp"

namespace RTL8139 {
    constexpr uint16_t REG_MAC        = 0x00;
    constexpr uint16_t REG_RBSTART    = 0x30;
    constexpr uint16_t REG_CMD        = 0x37;
    constexpr uint16_t REG_CAPR       = 0x38;
    constexpr uint16_t REG_IMR        = 0x3C;
    constexpr uint16_t REG_ISR        = 0x3E;
    constexpr uint16_t REG_TCR        = 0x40;
    constexpr uint16_t REG_RCR        = 0x44;
    constexpr uint16_t REG_CONFIG1    = 0x52;

    constexpr uint32_t RX_BUF_SIZE   = 8192 + 16 + 1500;
    constexpr uint32_t RCR_VAL       =
        (1 << 0) |   // AAP  accept all
        (1 << 1) |   // APM  accept physical match
        (1 << 2) |   // AM   accept multicast
        (1 << 3) |   // AB   accept broadcast
        (1 << 7) |   // WRAP buffer wrap
        (4 << 8);    // MXDMA 256 bytes

    extern uint16_t io_base;
    extern uint8_t mac[6];
    extern uint16_t rx_offset;
    extern uint8_t *rx_buffer;

    constexpr uint16_t REG_TSAD0 = 0x20;
    constexpr uint16_t REG_TSD0  = 0x10;

    extern uint8_t tx_slot;
    extern uint8_t *tx_buffers[4];
    extern uint8_t *tx_base;

    bool init();
    void handle_rx(const IDT::ISR_Registers *regs);
    bool send(const uint8_t* data, uint16_t length);

    struct EthernetHeader {
        uint8_t dst_mac[6];
        uint8_t src_mac[6];
        uint16_t ethertype;
    } __attribute__((packed));

    struct ARPHeader {
        uint16_t hardware_type; // Ethernet = 1
        uint16_t protocol_type; // IPv4 = 0x0800
        uint8_t mac_len; // 6
        uint8_t ip_len; // 4
        uint16_t opcode; // request=1, reply=2

        uint8_t sender_mac[6];
        uint32_t sender_ip;

        uint8_t target_mac[6];
        uint32_t target_ip;
    } __attribute__((packed));

    struct ARPPacket {
        EthernetHeader eth;
        ARPHeader arp;
    } __attribute__((packed));

    static uint16_t htons(const uint16_t x) {
        return (x << 8) | (x >> 8);
    }

    static uint32_t htonl(const uint32_t x) {
        return ((x << 24) & 0xFF000000) |
               ((x << 8)  & 0x00FF0000) |
               ((x >> 8)  & 0x0000FF00) |
               ((x >> 24) & 0x000000FF);
    }
}
