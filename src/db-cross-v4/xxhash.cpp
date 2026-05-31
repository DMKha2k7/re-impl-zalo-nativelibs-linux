#include "xxhash.h"
#include <cstring>

#define PRIME32_1 2654435761U
#define PRIME32_2 2246822519U
#define PRIME32_3 3266489917U
#define PRIME32_4  668265263U
#define PRIME32_5  374761393U

static inline uint32_t rotl32(uint32_t x, int r) {
    return (x << r) | (x >> (32 - r));
}

static inline uint32_t readLE32(const uint8_t* p) {
    uint32_t val;
    std::memcpy(&val, p, 4);
    return val;
}

uint32_t XXHash32::calculate(const void* input, size_t length, uint32_t seed) {
    const uint8_t* p = (const uint8_t*)input;
    const uint8_t* const bEnd = p + length;
    uint32_t h32;

    if (length >= 16) {
        const uint8_t* const limit = bEnd - 16;
        uint32_t v1 = seed + PRIME32_1 + PRIME32_2;
        uint32_t v2 = seed + PRIME32_2;
        uint32_t v3 = seed + 0;
        uint32_t v4 = seed - PRIME32_1;

        do {
            v1 += readLE32(p) * PRIME32_2;
            v1 = rotl32(v1, 13);
            v1 *= PRIME32_1;
            p += 4;

            v2 += readLE32(p) * PRIME32_2;
            v2 = rotl32(v2, 13);
            v2 *= PRIME32_1;
            p += 4;

            v3 += readLE32(p) * PRIME32_2;
            v3 = rotl32(v3, 13);
            v3 *= PRIME32_1;
            p += 4;

            v4 += readLE32(p) * PRIME32_2;
            v4 = rotl32(v4, 13);
            v4 *= PRIME32_1;
            p += 4;
        } while (p <= limit);

        h32 = rotl32(v1, 1) + rotl32(v2, 7) + rotl32(v3, 12) + rotl32(v4, 18);
    } else {
        h32 = seed + PRIME32_5;
    }

    h32 += (uint32_t)length;

    while (p + 4 <= bEnd) {
        h32 += readLE32(p) * PRIME32_3;
        h32 = rotl32(h32, 17) * PRIME32_4;
        p += 4;
    }

    while (p < bEnd) {
        h32 += (*p) * PRIME32_5;
        h32 = rotl32(h32, 11) * PRIME32_1;
        p++;
    }

    h32 ^= h32 >> 15;
    h32 *= PRIME32_2;
    h32 ^= h32 >> 13;
    h32 *= PRIME32_3;
    h32 ^= h32 >> 16;

    return h32;
}
