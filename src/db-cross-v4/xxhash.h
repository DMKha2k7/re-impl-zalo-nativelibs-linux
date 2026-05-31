#ifndef XXHASH_H
#define XXHASH_H

#include <cstdint>
#include <cstddef>

class XXHash32 {
public:
    static uint32_t calculate(const void* input, size_t length, uint32_t seed = 0);
};

#endif // XXHASH_H
