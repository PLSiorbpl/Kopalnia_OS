#pragma once
#include "libs/std/types.hpp"
#include "std/printf.hpp"

namespace heap {
	struct alignas(16) Block {
		uint64_t size;
		bool free;
		Block* next;
		Block* prev;
	};

	extern uint64_t heap_start;
	extern uint64_t heap_end;
	extern uint64_t heap_ptr;
	extern Block* heap_head;

	void heap_init(uint64_t size);

	void* malloc(uint64_t size);
	void* malloc_aligned(uint64_t size, uint64_t align, uint64_t boundry);

	void free(void* ptr);

	uint64_t check_free_heap();
	uint64_t check_used_heap();
	uint64_t check_heap();

	void dump_heap();
}

inline void* operator new(const size_t size) {
	return heap::malloc(size);
}

inline void operator delete(void* ptr) noexcept {
	heap::free(ptr);
}