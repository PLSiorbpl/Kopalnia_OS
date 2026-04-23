#pragma once
#include "std/types.hpp"

namespace USB {
    typedef struct xhci_transfer_request_block  {
        uint64_t parameter;
        uint32_t status;
        union {
            struct {
                uint32_t cycle_bit : 1;
                uint32_t rsvd      : 9;
                uint32_t trb_type  : 6;
                uint32_t rsvd1     : 16;
            };
            uint32_t control;
        };
    } xhci_trb_t;
    static_assert(sizeof(xhci_trb_t) == sizeof(uint32_t) * 4);
}