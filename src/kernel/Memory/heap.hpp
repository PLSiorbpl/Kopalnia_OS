#pragma once
#include "PLlib/types.hpp"

namespace heap {
	extern "C" char _end;
	extern "C" char start_;

	struct Block {
		uint64_t size;
		bool free;
		Block* next;
		Block* prev;
	};

	extern "C" Block heap_start_;

	extern uintptr_t heap_start;
	extern uintptr_t heap_end;
	extern uintptr_t heap_ptr;
	extern Block* heap_head;

	void heap_init(uintptr_t size);

	void* malloc(uint64_t size);

	void free(void* ptr);

	uint64_t check_free_heap();
	uint64_t check_used_heap();
	uint64_t check_heap();

	void dump_heap();
}