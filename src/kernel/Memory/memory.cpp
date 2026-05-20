#include "heap.hpp"

void* operator new(const size_t size) {
    return heap::malloc(size);
}

void operator delete(void* ptr) noexcept {
    heap::free(ptr);
}

void* operator new(size_t, void* ptr) noexcept {
    return ptr;
}

void operator delete(void*, void*) noexcept {}

void operator delete(void* ptr, size_t size) noexcept {
    heap::free(ptr);
}

// Array
void* operator new[](const size_t size) {
    return heap::malloc(size);
}

void operator delete[](void* ptr) noexcept {
    heap::free(ptr);
}

void operator delete[](void* ptr, size_t size) noexcept {
    heap::free(ptr);
}