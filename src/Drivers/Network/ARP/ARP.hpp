#pragma once
#include "Drivers/Network/Net_Device.hpp"
#include "std/types.hpp"

namespace NET {
    constexpr uint16_t ARP_REQUEST = 1;
    constexpr uint16_t ARP_REPLY   = 2;

    void receive_ARP(Net_Device *dev, const uint8_t *frame, uint16_t len);
}
