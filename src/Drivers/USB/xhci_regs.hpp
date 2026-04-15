#pragma once
#include "PLlib/types.hpp"

namespace USB {
    struct xhci_doorbell_register {
        union {
            struct {
                uint8_t db_target;
                uint8_t rsvd;
                uint16_t db_stream_id;
            };

            uint32_t raw;
        };
    };

    class xhci_doorbell_manager {
    public:
        void construct(uint64_t base);

        void ring_doorbell(uint8_t doorbell, uint8_t target);

        void ring_command_doorbell();

        void ring_controll_endpoint_doorbell(uint8_t doorbell);
    private:
        xhci_doorbell_register *m_doorbell_registers;
    };
}