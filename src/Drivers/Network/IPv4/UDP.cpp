#include "UDP.hpp"
#include "Drivers/Network/Common.hpp"

namespace NET {
    void receive_udp(Net_Device *dev, const uint8_t *frame, uint16_t len) {
        auto *packet = (IPv4Packet *)frame;

        const uint8_t ip_header_len = (packet->ip.ihl_version & 0x0F) * 4;

        auto* udp = (UDPHeader *)(frame + 14 + ip_header_len);

        if (udp->checksum == 0)
            log::warn("[ NET ] UDP packet whitout checksum");

        log::info("[ NET ] UDP from port: %u -> %u", (uint32_t)Bswap_16(udp->src_port), (uint32_t)Bswap_16(udp->dst_port));

        uint16_t payload_len = Bswap_16(udp->length) - sizeof(UDPHeader);

        uint8_t* payload = (uint8_t*)udp + sizeof(UDPHeader);

        log::success("UDP payload: %s", payload);
    }
}
