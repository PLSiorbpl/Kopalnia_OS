#pragma once
#include "Drivers/Network/Common.hpp"
#include "Drivers/Network/Net_Device.hpp"

namespace NET {
    void receive_udp(Net_Device *dev, const uint8_t *frame, uint16_t len);
}
