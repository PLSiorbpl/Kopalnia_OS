#include "partition_manager.h"

#include "kernel/log.h"

namespace fs::partition {
    partition_manager::partition_manager() : header_partition() { }

    void partition_manager::init(const drivers::ahci::ahci_device& dev) {
        u16 buf[256];
        dev.read(1, 1, buf);
        const auto* header = reinterpret_cast<gpt_header*>(buf);

        if (buf[0] == 0 && buf[67] == 0) {
            log::error("Buf probably all 0 for some reason");
        }

        if (mem::memcmp(&header->signature, "EFI PART", 8) == false) {
            log::warn("Failed to find signature bytes for GPT partition.");
            return;
        }

        log::success("Found partition header!");

        const u32 total_size = header->partition_entry_size * header->partition_entry_count;
        const u32 sectors = (total_size + 511) / 512;

        std::vector<u16> partitions_buf;
        partitions_buf.resize(sectors * 256);
        partitions_buf.size = sectors * 256;
        dev.read(header->partition_entry_lba, sectors, partitions_buf.data);

        for (u32 i = 0; i < header->partition_entry_count; i++) {
            auto* entry = reinterpret_cast<gpt_partition*>(reinterpret_cast<u8*>(partitions_buf.data) + i * header->partition_entry_size);

            if (mem::memcmp(entry->type_guid, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16) == true)
                continue;

            char name_buf[37] = {};
            for (int j = 0; j < 36; j++) {
                u16 wc = entry->name[j];
                if (wc == 0) break;
                name_buf[j] = static_cast<char>(wc & 0xFF);
            }

            log::info("Found partition %i with name of '%s'", i, name_buf);
            partitions.push_back(*entry);
        }
    }
}
