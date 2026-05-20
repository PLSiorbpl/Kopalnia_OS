#pragma once
#include "std/types.hpp"

namespace NET {
    constexpr uint16_t ARP_Ether_Type = 0x0806;
    constexpr uint16_t IPv4_Ether_Type = 0x0800;
    constexpr uint16_t IPv4_Protocol_ICMP = 1;
    constexpr uint16_t IPv4_Protocol_UDP = 17;

    uint16_t Bswap_16(uint16_t x);

    uint32_t Bswap_32(uint32_t x);

    // 192, 168, 0, 1
    uint32_t make_ipv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

    void ipv4_to_str(uint32_t ip, char *out, bool reverse = false);

    uint16_t checksum(const void* data, int len);

    struct EthernetHeader {
        uint8_t dst_mac[6];
        uint8_t src_mac[6];
        uint16_t ethertype;
    } __attribute__((packed));

    // ----------------------------------------------
    // ARP
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

    // ----------------------------------------------
    // IPv4
    struct IPv4Header {
        uint8_t  ihl_version;
        uint8_t  tos;
        uint16_t total_length;
        uint16_t id;
        uint16_t flags_frag;
        uint8_t  ttl;
        uint8_t  protocol;
        uint16_t checksum;
        uint32_t src_ip;
        uint32_t dst_ip;
    } __attribute__((packed));

    struct ICMPHeader {
        uint8_t type;
        uint8_t code;
        uint16_t checksum;
        uint16_t id;
        uint16_t seq;
    } __attribute__((packed));

    struct UDPHeader {
        uint16_t src_port;
        uint16_t dst_port;
        uint16_t length;
        uint16_t checksum;
    } __attribute__((packed));
}
