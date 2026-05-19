#pragma once
#include "Net_Device.hpp"
#include "std/types.hpp"

namespace NET {
    void receive_ethernet(Net_Device *dev,const uint8_t* frame,uint16_t len);

    void send_ethernet(Net_Device *dev, const uint8_t *dst_mac, uint16_t ethertype, const void *payload, uint16_t len);
}