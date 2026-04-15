#pragma once

namespace std {
    template<typename T>
    struct vector {
        uint64_t *data;
        uint64_t size;
        uint64_t capacity;
    };
}