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


uint64_t HashCRC32_C(const void* mem, size_t size) {
    uint32_t cur_hash = 0xDEADDEAD;
    for (int i = 0; i < size; i++) {
        cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[i]);
    }

    return cur_hash;
}


uint64_t HashCRC32_C_unroll(const void* mem, size_t size) {
    uint32_t cur_hash = 0xDEADDEAD;
    switch (size) {
        case 16: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[15]);
        case 15: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[14]);
        case 14: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[13]);
        case 13: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[12]);
        case 12: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[11]);
        case 11: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[10]);
        case 10: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[9]);
        case  9: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[8]);
        case  8: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[7]);
        case  7: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[6]);
        case  6: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[5]);
        case  5: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[4]);
        case  4: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[3]);
        case  3: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[2]);
        case  2: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[1]);
        case  1: cur_hash = _mm_crc32_u8(cur_hash, ((const uint8_t*)mem)[0]);
    }

    return cur_hash;
}


#include <string.h>
uint64_t HashCRC32_inline(const void* data, size_t length) {
    uint64_t hash = 0;

    alignas(16) char str[16] = {};

    memcpy(str, data, length);

    asm(
        "crc32 %[hash], qword ptr [%[str] + 0]\n\t"
        "crc32 %[hash], qword ptr [%[str] + 8]\n\t"
        : [hash] "+r" (hash)
        : [str] "r" (str)
        : "memory"
    );
    
    return hash;
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


// source: https://ru.wikipedia.org/wiki/MurmurHash2
uint64_t HashMurmur2(const void* mem, size_t size) 
{
    const size_t m = 0x5bd1e995;
    const size_t seed = 0;
    const char* key = (const char*)mem;
    const int r = 24;

    uint64_t h = seed ^ size;

    const unsigned char * data = (const unsigned char *)key;
    uint64_t k = 0;

    while (size >= 4)
    {
        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        size -= 4;
    }

    switch (size)
    {
        case 3:
            h ^= data[2] << 16;
        case 2:
            h ^= data[1] << 8;
        case 1:
            h ^= data[0];
            h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}
