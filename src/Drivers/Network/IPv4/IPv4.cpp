#include "IPv4.hpp"

#include "UDP.hpp"
#include "Drivers/Network/Common.hpp"
#include "Drivers/Network/Net_Device.hpp"
#include "Drivers/Network/IPv4/ICMP.hpp"
#include "kernel/log.h"

namespace NET {
    void receive_IPv4(Net_Device *dev, const uint8_t *frame, const uint16_t len) {
        if (len < sizeof(IPv4Header) + sizeof(EthernetHeader)) {
            log::warn("IPv4 packet too short");
            return;
        }

        const auto ip = (IPv4Header *)(frame + sizeof(EthernetHeader));
        if (ip->dst_ip != Bswap_32( dev->get_ipv4() )) return; // not to us

        switch (ip->protocol) {
            case IPv4_Protocol_ICMP: {
                receive_ICMP(dev, frame, len);
                return;
            }
            case IPv4_Protocol_UDP: {
                receive_udp(dev, frame, len);
                return;
            }
            default:
                return;
        }
    }
}
