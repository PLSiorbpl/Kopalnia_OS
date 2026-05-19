#include "ICMP.hpp"
#include "Drivers/Network/Common.hpp"
#include "Drivers/Network/Ethernet.hpp"
#include "Drivers/Network/Net_Device.hpp"
#include "kernel/log.h"
#include "std/mem_common.hpp"

namespace NET {
    void receive_ICMP(Net_Device *dev, const uint8_t *frame, uint16_t len) {
        if (len < sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(ICMPHeader)) {
            log::warn("[ NET ] ICMP packet too short");
            return;
        }
        auto *eth = (EthernetHeader *)frame;

        auto *ip = (IPv4Header *)(frame + sizeof(EthernetHeader));
        const uint8_t ip_header_len = (ip->ihl_version & 0x0F) * 4;

        auto *icmp = (ICMPHeader *)(frame + sizeof(EthernetHeader) + ip_header_len);

        char buf[16];
        ipv4_to_str(ip->src_ip, buf, true);
        log::info("icmp from: %s", buf);

        if (icmp->type == 8) { // Echo request
            // This should be its own function fr send_ICMP()

            const uint32_t tmp_ip = ip->src_ip;
            ip->src_ip = ip->dst_ip;
            ip->dst_ip = tmp_ip;

            ip->checksum = 0;
            ip->checksum = checksum(ip, ip_header_len);

            icmp->type = 0; // Reply
            icmp->code = 0;

            icmp->checksum = 0;
            icmp->checksum = checksum(icmp, Bswap_16(ip->total_length) - ip_header_len);

            send_ethernet(dev, eth->src_mac, Bswap_16(IPv4_Ether_Type), ip, Bswap_16(ip->total_length));
        }
    }
}
