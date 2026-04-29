#include "partition_manager.h"

#include "kernel/log.h"

namespace fs::partition {
    partition_manager::partition_manager() : header_partition() { }

    void partition_manager::init(const drivers::ahci::ahci_device& dev) {
        const auto buf = static_cast<u16*>(heap::malloc_align(512, 4));
        dev.read(1, 1, buf);
        const auto* header = reinterpret_cast<gpt_header*>(buf);

        if (mem::memcmp(&header->signature, "EFI PART", 8) == false) {
            log::warn("Failed to find signature bytes for GPT partition.");
            heap::free_align(buf);
            return;
        }

        log::success("Found partition header!");

        const u32 total_size = header->partition_entry_size * header->partition_entry_count;
        const u32 sectors = (total_size + 511) / 512;

        const auto partitions_buf = static_cast<u16*>(heap::malloc_align(total_size, 4));
        if (!dev.read(header->partition_entry_lba, sectors, partitions_buf)) {
            heap::free_align(partitions_buf);
            heap::free_align(buf);
            return;
        }

        for (u32 i = 0; i < header->partition_entry_count; i++) {
            const auto* entry = reinterpret_cast<gpt_partition*>(reinterpret_cast<u8*>(partitions_buf) + i * header->partition_entry_size);
            auto* guid64 = reinterpret_cast<const u64*>(entry->type_guid);
            if (guid64[0] == 0 && guid64[1] == 0)
                continue;
            if (entry->starting_lba == 0)
                continue;
            if (entry->name[0] == 0)
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

        heap::free_align(partitions_buf);
        heap::free_align(buf);
    }
}
