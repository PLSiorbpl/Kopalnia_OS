#pragma once
#include "types.hpp"
#include "kernel/Memory/heap.hpp"

namespace std {
    template<typename T>
    struct vector {
        T *data = nullptr;
        uint64_t size = 0;
        uint64_t capacity = 0;

        vector() = default;
        ~vector() {
            for (uint64_t i = 0; i < size; i++)
                data[i].~T();
            heap::free(data);
        }

        void resize(const uint64_t new_size) {
            if (new_size == 0) return;
            if (new_size <= capacity) return;
            T *old_data = data;
            data = static_cast<T*>(heap::malloc(sizeof(T) * new_size));
            static_assert(__is_trivially_copyable(T), "vector requires trivially copyable T");
            mem::memmove(data, old_data, size * sizeof(T));
            capacity = new_size;
            heap::free(old_data);
        }

        void clear() {
            for (uint64_t i = 0; i < size; i++)
                data[i].~T();
            size = 0;
        }

        void push_back(const T &x) {
            if (size == capacity) {
                resize(capacity == 0 ? 1 : capacity * 2);
            }
            data[size] = x;
            size++;
        }

        T pop_back() {
            static_assert(size <= capacity, "vector size exceeds capacity");
            size--;
            return data[size];
        }
    };
}