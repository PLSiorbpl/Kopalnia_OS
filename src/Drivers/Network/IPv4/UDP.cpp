#include "UDP.hpp"
#include "Drivers/Network/Common.hpp"
#include "kernel/Memory/heap.hpp"
#include "std/mem_common.hpp"
#include "kernel/log.h"

namespace NET {
    void receive_udp(Net_Device *dev, const uint8_t *frame, uint16_t len) {
        const auto *ip = (IPv4Header *)(frame + sizeof(EthernetHeader));
        const uint8_t ip_header_len = (ip->ihl_version & 0x0F) * 4;

        auto* udp = (UDPHeader *)(frame + sizeof(EthernetHeader) + ip_header_len);

        // TODO
        // Implement checksum checking

        const uint16_t payload_len = Bswap_16(udp->length) - sizeof(UDPHeader);

        auto *buf = static_cast<uint8_t *>(heap::malloc(payload_len + 1));
        const uint8_t* payload = reinterpret_cast<uint8_t *>(udp) + sizeof(UDPHeader);
        mem::memcpy(buf, payload, payload_len);

        buf[payload_len] = '\0';

        log::info("[ NET ] UDP from port: %u -> %u payload: &e%s", static_cast<uint32_t>(Bswap_16(udp->src_port)),
                  static_cast<uint32_t>(Bswap_16(udp->dst_port)), buf);
        heap::free(buf);
    }
}
