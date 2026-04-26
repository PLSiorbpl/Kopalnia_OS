#include "Paging.hpp"

#include "Memory/heap.hpp"
#include "../libs/std/mem_common.hpp"
#include "../libs/std/types.hpp"

namespace Paging {
    alignas(4096) uint64_t PML4[512];

    void Init() {
        for (uint64_t& i : PML4)
            i = 0;
    }

    uint64_t alloc_page() {
        const auto addr = reinterpret_cast<uint64_t>(heap::malloc(4096 + 4095));

        const uint64_t aligned = (addr + 4095) & ~4095ULL;

        return aligned;
    }

    // TODO
    // fix being able to map only 0-136MB
    void Map_memory(uint64_t start, uint64_t end, const uint64_t flags) {
        start = start & ~(4095);
        end = (end + 4095) & ~(4095);

        for (uint64_t addr = start; addr < end; addr += 4096) {
            const uint64_t pml4_i = (addr >> 39) & 0x1FF;
            const uint64_t pdpt_i = (addr >> 30) & 0x1FF;
            const uint64_t pd_i = (addr >> 21) & 0x1FF;
            const uint64_t pt_i   = (addr >> 12) & 0x1FF;

            if (!(PML4[pml4_i] & Present)) {
                const uint64_t new_table = alloc_page();
                mem::memset(reinterpret_cast<void*>(new_table), 0, 4096);
                PML4[pml4_i] = new_table | Present | Writable | User;
            }

            auto *PDPT = reinterpret_cast<uint64_t *>(PML4[pml4_i] & ~0xFFFULL);
            if (!(PDPT[pdpt_i] & Present)) {
                const uint64_t new_table = alloc_page();
                mem::memset(reinterpret_cast<void*>(new_table), 0, 4096);
                PDPT[pdpt_i] = new_table | Present | Writable | User;
            }

            auto *PD = reinterpret_cast<uint64_t *>(PDPT[pdpt_i] & ~0xFFFULL);
            if (!(PD[pd_i] & Present)) {
                const uint64_t new_table = alloc_page();
                mem::memset(reinterpret_cast<void*>(new_table), 0, 4096);
                PD[pd_i] = new_table | Present | Writable | User;
            }

            auto *PT = reinterpret_cast<uint64_t *>(PD[pd_i] & ~0xFFFULL);
            PT[pt_i] = addr | Present | Writable | flags; // Physical == Virtual (identity mapping)
            asm volatile("invlpg (%0)" :: "r"(addr) : "memory");
        }
    }

    void Enable_paging() {
        // We only need to update address of PML4 because we enabled it before in elevate.asm
        auto pml4_addr = reinterpret_cast<uint64_t>(PML4);
        asm volatile("mov %0, %%cr3" :: "r"(pml4_addr));
    }
}
