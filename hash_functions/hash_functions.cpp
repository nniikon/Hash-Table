#include "hash_functions.h"
#include <assert.h>
#include <immintrin.h>

uint64_t HashZero(const void* mem, size_t size) {
    return 0;
}


uint64_t HashLength(const void* mem, size_t size) {
    return size;
}


uint64_t HashSum(const void* mem, size_t size) {
    uint64_t hash = 0;
    const char* c_mem = (const char*)mem;

    for (size_t i = 0; i < size; i++) {
        hash += (uint64_t)c_mem[i];
    }
    return hash;
}


uint64_t HashNormalSum(const void* mem, size_t size) {
    return HashSum(mem, size) / size;
}


enum RotateDirection {
    RIGHT,
    LEFT,
};

const uint64_t bits_in_byte = 8;

inline static uint64_t RotateRight(uint64_t num) {
    return ((num >> 1) | (num << (sizeof(num) * bits_in_byte - 1)));
}

inline static uint64_t RotateLeft(uint64_t num) {
    return ((num << 1) | (num >> (sizeof(num) * bits_in_byte - 1)));
}


uint64_t HashRotate(const void* mem, size_t size, RotateDirection direction) {
    uint64_t hash = 0;
    const char* c_mem = (const char*)mem;

    for (size_t i = 0; i < size; i++) {
        // rotate right
        switch(direction) {
            case RIGHT: hash = RotateRight(hash);
            case  LEFT: hash = RotateLeft (hash);
            default: assert(0);
        }
        // xor
        hash = hash ^ (uint64_t)c_mem[i];
    }

    return hash;

}


uint64_t HashRotateRight(const void* mem, size_t size) {
    return HashRotate(mem, size, RIGHT);
}


uint64_t HashRotateLeft(const void* mem, size_t size) {
    return HashRotate(mem, size, LEFT);
}


uint64_t HashCrc(const void* mem, size_t size) {
    uint64_t hash = 0xDEADDEADDEADDEAD;
    const char* c_mem = (const char*)mem;

    for (int i = 0; i < size; i++) {
        hash = _mm_crc32_u64(hash, (uint64_t)((c_mem)[i]));
    }

    return hash;
}

