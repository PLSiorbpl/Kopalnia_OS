#pragma once
#include "../libs/std/types.hpp"

namespace Paging {
    #define PAGE_SIZE 4096

    constexpr uint64_t None         = 0;
    constexpr uint64_t Present      = (1ULL << 0);
    constexpr uint64_t Writable     = (1ULL << 1);
    constexpr uint64_t User         = (1ULL << 2);
    constexpr uint64_t WriteThrough = (1ULL << 3);
    constexpr uint64_t CacheDisable = (1ULL << 4);
    constexpr uint64_t Accessed     = (1ULL << 5);
    constexpr uint64_t Dirty        = (1ULL << 6);
    constexpr uint64_t LargePage    = (1ULL << 7);
    constexpr uint64_t PAT          = (1ULL << 7);
    constexpr uint64_t Global       = (1ULL << 8);
    constexpr uint64_t NoExecute    = (1ULL << 63);

    extern uint64_t PML4[512];

    void Init();
    uint64_t alloc_page();
    void Map_memory(uint64_t start, uint64_t end, uint64_t flags = 0);
    void Enable_paging();

    namespace Profile {
        constexpr uint64_t KernelCode =  Global;
        constexpr uint64_t KernelData =  (NoExecute | Global);
        constexpr uint64_t KernelStack = NoExecute;
        constexpr uint64_t UserCode =    User;
        constexpr uint64_t UserData =    (User | NoExecute);
        constexpr uint64_t MMIO =        (CacheDisable | NoExecute);
        constexpr uint64_t VramWC =      (CacheDisable | NoExecute);
    }
}