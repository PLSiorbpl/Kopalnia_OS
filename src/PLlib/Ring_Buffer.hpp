#pragma once
#include "PLlib/types.hpp"

namespace mem {
    template<typename T, size_t size>
    struct Ring_Buffer {
        T data[size] = {};
        size_t head = 0;
        size_t tail = 0;

        [[nodiscard]] bool empty() const {
            return tail == head;
        }

        [[nodiscard]] bool full() const {
            return ((head + 1) % size) == tail;
        }

        void clear() {
            for (size_t i = 0; i < size; i++)
                data[i] = {};
            head = 0;
            tail = 0;
        }

        bool push(const T &value) {
            const size_t next = (head + 1) % size;
            if (next == tail) return false;
            data[head] = value;
            head = next;
            return true;
        }

        bool pop(T &out) {
            if (tail == head) return false;
            out = data[tail];
            tail = (tail + 1) % size;
            return true;
        }
    };
}