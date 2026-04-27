#pragma once
#include "ahci_helper.h"
#include "ahci_port.h"
#include "kernel/Sleep.hpp"

namespace drivers::ahci {
    class ahci_device {
    public:
        explicit ahci_device(ahci_port* port, const bool is_active) : port(port), active(is_active) { }
        ~ahci_device() = default;

        bool initialize();
        bool read(const u64 start, const u32 count, u16* buffer) const {
            return port->read(start, count, buffer, sector_size);
        }

        bool write(const u64 start, const u32 count, const u16* buffer) const {
            return port->write(start, count, buffer, sector_size);
        }

        [[nodiscard]] const char* get_model() const { return model; }
        [[nodiscard]] const char* get_firmware() const { return firmware; }
        [[nodiscard]] u64 get_sector_count() const { return sector_count; }
        [[nodiscard]] u64 get_sector_size() const { return sector_size; }
        [[nodiscard]] bool is_active() const { return active; }
    private:
        ahci_port* port = nullptr;

        char model[41] = {};
        char firmware[9] = {};
        u64 sector_count = 0;
        u32 sector_size = 512;
        bool active = false;
    };
}
