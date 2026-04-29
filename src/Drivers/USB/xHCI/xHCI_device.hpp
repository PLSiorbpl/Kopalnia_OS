#pragma once
#include "xHCI_device_ctx.hpp"
#include "xHCI_rings.hpp"
#include "std/types.hpp"

namespace USB {

    class xhci_device {
    public:
        // port is a 1-based port ID
        xhci_device(uint8_t port, uint8_t slot, uint8_t speed, bool use_64byte_ctx);
        ~xhci_device() = default;

        uint8_t get_port() const { return m_port; }
        uint8_t get_port_regset_idx() const { return m_port - 1; }
        uint8_t get_slot() const { return m_slot; }
        uint8_t get_speed() const { return m_speed; }
        uint32_t route_string() const { return m_route_string; }

        void set_root_port_id(uint8_t rp) { m_root_port_id = rp; }
        void  set_output_ctx(void* ctx) { m_output_ctx = ctx; }
        void* output_ctx() const { return m_output_ctx; }

        void* ctrl_transfer_buffer() const { return m_ctrl_transfer_buffer; }
        uintptr_t ctrl_transfer_buffer_phys() const { return m_ctrl_transfer_buffer_phys; }

        uintptr_t get_input_ctx_dma() const { return m_input_ctx_dma_addr; }

        xhci_transfer_ring* ctrl_ring() { return m_ctrl_ring; }

        xhci_input_control_context32*   get_input_ctrl_ctx();
        xhci_slot_context32*            get_input_slot_ctx();
        xhci_endpoint_context32*        get_input_ctrl_ep_ctx();
        xhci_endpoint_context32*        get_input_ep_ctx(uint8_t endpoint_num);

        // Copies data from the output device context into the input context
        void sync_input_ctx(void* out_ctx);

    private:
        const uint8_t m_port = 0;           // 1-based port ID
        const uint8_t m_slot = 0;           // slot index in the xhci DCBAA
        const uint8_t m_speed = 0;          // port speed
        const bool    m_use64byte_ctx;

        uint32_t  m_route_string = 0;
        uint8_t   m_root_port_id = 0;

        void*     m_output_ctx = nullptr;
        void*         m_input_ctx;
        uintptr_t     m_input_ctx_dma_addr;

        void*     m_ctrl_transfer_buffer = nullptr;
        uintptr_t m_ctrl_transfer_buffer_phys = 0;

        xhci_transfer_ring* m_ctrl_ring = nullptr;

    private:
        void _alloc_input_ctx();
    };
}
