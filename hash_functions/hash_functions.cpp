#include "hash_functions.h"
#include <assert.h>
#include <immintrin.h>

uint64_t HashZero(const void* mem, size_t size) {
    return 0;
}


uint64_t HashLength(const void* mem, size_t size) {
    return size;
}


uint64_t HashLetter(const void* mem, size_t size) {
    return (uint64_t)((const char*)mem)[0];
}


uint64_t HashSum(const void* mem, size_t size) {
    const char* c_mem = (const char*)mem;

    uint64_t hash = 0;
    for (size_t i = 0; i < size; i++) {
        hash += (uint64_t)c_mem[i];
    }

    return hash;
}


uint64_t HashNormalSum(const void* mem, size_t size) {
    return HashSum(mem, size) / size;
}


const uint64_t kBitsInByte = 8;

inline static uint64_t RotateRight(uint64_t num) {
    return ((num >> 1) | (num << (sizeof(num) * kBitsInByte - 1)));
}

inline static uint64_t RotateLeft(uint64_t num) {
    return ((num << 1) | (num >> (sizeof(num) * kBitsInByte - 1)));
}


//                                                left otherwise
uint64_t HashRotate(const void* mem, size_t size, bool isRight) {
    const char* c_mem = (const char*)mem;
    uint64_t hash = (uint64_t)c_mem[0];

    for (size_t i = 0; i < size; i++) {
        // rotate right
        hash = (isRight) ? RotateRight(hash) : RotateLeft(hash);
        // xor
        hash = hash ^ (uint64_t)c_mem[i];
    }

    return hash;
}


uint64_t HashRotateRight(const void* mem, size_t size) {
    return HashRotate(mem, size, 1);
}


uint64_t HashRotateLeft(const void* mem, size_t size) {
    return HashRotate(mem, size, 0);
}


uint64_t HashCRC32(const void* mem, size_t size) {
    static uint32_t prev_hash = 0xDEADDEAD;
    uint32_t cur_hash = prev_hash;
    for (int i = 0; i < size; i++) {
        cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[i]);
    }

    return cur_hash;
}


uint64_t HashKR(const void* mem, size_t size) {
    uint64_t hash = 0;
    const char* c_mem = (const char*)mem;

    for (size_t i = 0; i < size; i++) {
        hash <<= 5;
        hash ^= (hash >> 27);
        hash ^= (uint64_t)c_mem[i];
    }

    return hash;
}


uint64_t HashJenkins(const void* mem, size_t size) {
    const char* c_mem = (const char*)mem;

    uint64_t hash = 0;
    for (size_t i = 0; i < size; i++) {
        hash += (uint64_t)c_mem[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}


uint64_t HashElf(const void* mem, size_t size) {
    uint64_t h = 0;
    uint64_t high = 0;

    const char* c_mem = (const char*)mem;

    for (int i = 0; i < size; i++)
    {
        h = (h << 4) + (uint64_t)c_mem[i];
        high = h & 0xF0000000;

        if (high != 0) {
            h ^= high >> 24;
            h &= ~high;
        }
    }
    return h;
}
