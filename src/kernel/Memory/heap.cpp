#include "heap.hpp"
#include "PLlib/String_common.hpp"

namespace heap {
    Block* heap_head;
    uint64_t heap_start;
    uint64_t heap_end;
    uint64_t heap_ptr;

    void heap_init(const uint64_t size) {
        heap_head = &heap_start_;
        heap_head->size = size - sizeof(Block);
        heap_head->free = true;
        heap_head->next = nullptr;
        heap_head->prev = nullptr;

        heap_start = reinterpret_cast<uint64_t>(&_end);
        heap_end = heap_start + size;
    }
    // True allocator
    void* malloc(uint64_t size) {
        size = (size + 7) & ~7;
        for (Block* block = heap_head; block != nullptr; block = block->next) {
            if (block->free && block->size >= size) {

                if (block->size >= size + sizeof(Block) + 8) {
                    // Split
                    const uint64_t new_size = block->size - size - sizeof(Block);
                    auto* new_block = reinterpret_cast<Block *>(reinterpret_cast<uint8_t *>(block) + sizeof(Block) + size);
                    new_block->size = new_size;
                    new_block->free = true;
                    new_block->next = block->next;
                    new_block->prev = block;
                    if (new_block->next)
                        new_block->next->prev = new_block;
                    block->size = size;
                    block->next = new_block;
                }

                block->free = false;
                return block+1;
            }
        }
        return nullptr;
    }

    void* malloc_aligned(uint64_t size, uint64_t align, uint64_t boundary) {
        if (align == 0) align = 1;

        const uint64_t total = size + align + (boundary ? boundary : 0);

        const auto raw = reinterpret_cast<uint64_t>(malloc(total));
        if (!raw) return nullptr;

        // align
        uint64_t aligned = (raw + align - 1) & ~(align - 1);

        // boundary check
        if (boundary) {
            const uint64_t start_block = aligned & ~(boundary - 1);
            const uint64_t end_block   = (aligned + size - 1) & ~(boundary - 1);

            if (start_block != end_block) {
                aligned = (aligned + boundary) & ~(boundary - 1);
            }
        }

        return reinterpret_cast<void *>(aligned);
    }

    void free(void* ptr) {
        if (!ptr) return;
        Block* old_block = static_cast<Block*>(ptr) - 1;
        old_block->free = true;

        while (old_block->next && old_block->next->free) {
            // Merge Right
            const Block* next = old_block->next;
            old_block->size += next->size + sizeof(Block);
            old_block->next = next->next;
            if (old_block->next)
                old_block->next->prev = old_block;
        }

        while (old_block->prev && old_block->prev->free) {
            old_block->prev->size += old_block->size + sizeof(Block);
            old_block->prev->next = old_block->next;
            if (old_block->next)
                old_block->next->prev = old_block->prev;
            old_block = old_block->prev;
        }
    }

    uint64_t check_heap() {
        uint64_t free_bytes = 0;
        for (const Block* b = heap_head; b; b = b->next) {
            free_bytes += b->size;
        }
        return free_bytes;
    }

    uint64_t check_free_heap() {
        uint64_t free_bytes = 0;
        for (const Block* b = heap_head; b; b = b->next) {
            if (b->free) free_bytes += b->size;
        }
        return free_bytes;
    }

    uint64_t check_used_heap() {
        uint64_t free_bytes = 0;
        for (const Block* b = heap_head; b; b = b->next) {
            if (!b->free) free_bytes += b->size;
        }
        return free_bytes;
    }

    void dump_heap() {
        int b_count = 0;
        term::print("Heap Visualization\n", term::Color::LightCyan);
        for (Block* b = heap_head; b; b = b->next) {
            b_count++;
            term::print("\tBlock #");
            term::print_number(b_count);
            term::print(" @ ");
            term::print_hex(reinterpret_cast<uintptr_t>(b), term::Color::LightGray);
            term::print(" size: ");
            term::print_number(b->size, term::Color::Green);
            term::print("B");
            term::print(b->free ? " free\n" : " used\n", b->free ? term::Color::LightGreen : term::Color::LightRed);
        }
        term::print("\tBlocks total: ");
        term::print_number(b_count);
        term::print("\nSummary (");
        term::print("used", term::Color::LightRed); term::print("/");
        term::print("free", term::Color::LightGreen); term::print("/");
        term::print("all", term::Color::LightCyan); term::print("): ");
        term::print_number(heap::check_used_heap(), term::Color::LightRed);
        term::print("B / ");
        term::print_number(heap::check_free_heap(), term::Color::LightGreen);
        term::print("B / ");
        term::print_number(heap::check_heap(), term::Color::LightCyan);
        term::print("B\n\n");
    }
}
