#include "string.h"
#include "types.hpp"

namespace std {
    bool str_cmp(const char *str1, const char *str2) {
        uint64_t idx = 0;
        while (str1[idx] == str2[idx]) {
            if (str1[idx] == '\0') return true;
            idx++;
        }
        return false;
    }

    int strlen(const char* text) {
        int len = 0;
        while (text[len] != '\0') {
            len++;
        }
        return len;
    }
}
