#include "heap.hpp"

namespace heap {
    uintptr_t heap_start;
    uintptr_t heap_end;
    uintptr_t heap_ptr;

    void heap_init(const uintptr_t size) {
        heap_start = reinterpret_cast<uintptr_t>(&_end);
        heap_end = heap_start + size;
        heap_ptr = heap_start;
    }
    // Bump allocator
    void* malloc(const size_t size) {
        if (size > heap_end - heap_ptr || size <= 0) return nullptr;
        const uintptr_t ret = (heap_ptr + 7) & ~7; // align 8
        heap_ptr = ret + size;
        return reinterpret_cast<void*>(ret);
    }
}
