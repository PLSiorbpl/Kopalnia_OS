#pragma once
#include "gpt_partition.h"
#include "Drivers/achi/ahci_device.h"
#include "std/vector.hpp"

namespace fs::partition {
    class partition_manager {
    public:
        partition_manager();
        ~partition_manager() = default;

        void init(const drivers::ahci::ahci_device& dev);
    private:
        [[nodiscard]] bool validate_gpt() const;
        [[nodiscard]] bool validate_gpt_partition_entries(const u16* entries_buf) const;

        gpt_header* header;
        std::vector<gpt_partition> partitions;
    };
}
