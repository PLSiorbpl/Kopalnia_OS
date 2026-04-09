#pragma once
#include "PLlib/types.hpp"

namespace Paging {
    #define None         0
    #define Present      (1ULL << 0)
    #define Writable     (1ULL << 1)
    #define User         (1ULL << 2)
    #define WriteThrough (1ULL << 3)
    #define CacheDisable (1ULL << 4)
    #define Accessed     (1ULL << 5)
    #define Dirty        (1ULL << 6)
    #define LargePage    (1ULL << 7)
    #define PAT          (1ULL << 7)
    #define Global       (1ULL << 8)
    #define NoExecute    (1ULL << 63)

    extern uint64_t PML4[512];

    void Init();
    uint64_t alloc_page();
    void Map_memory(uint64_t start, uint64_t end, uint64_t flags = 0);
    void Enable_paging();

    namespace Profile {
        #define KernelCode  Present | Global;
        #define KernelData  Present | Writable | NoExecute | Global;
        #define KernelStack Present | Writable | NoExecute;
        #define UserCode    Present | User;
        #define UserData    Present | Writable | User | NoExecute;
        #define MMIO        Present | Writable | CacheDisable | NoExecute;
        #define VramWC      Present | Writable | WriteThrough | NoExecute;
    }
}