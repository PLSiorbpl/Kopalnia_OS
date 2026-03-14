#pragma once
#include "PLlib/types.hpp"

namespace heap {
	extern "C" char _end;

	extern uintptr_t heap_start;
	extern uintptr_t heap_end;
	extern uintptr_t heap_ptr;

	void heap_init(uintptr_t size);

	void* malloc(size_t size);
}