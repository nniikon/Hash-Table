#ifndef HASH_FUNCTIONS_H_
#define HASH_FUNCTIONS_H_

#include <stdlib.h>
#include <inttypes.h>

struct HashFunction {
    uint64_t (*hash_func)(const void* mem, size_t size);
    const char* description;
};

uint64_t HashZero       (const void* mem, size_t size);
uint64_t HashLength     (const void* mem, size_t size);
uint64_t HashSum        (const void* mem, size_t size);
uint64_t HashNormalSum  (const void* mem, size_t size);
uint64_t HashRotateRight(const void* mem, size_t size);
uint64_t HashRotateLeft (const void* mem, size_t size);
uint64_t HashCrc        (const void* mem, size_t size);

const HashFunction gHashFunctions[] = 
{
    {HashZero,        "Zero Hash"},
    {HashLength,      "Length Hash"},
    {HashSum,         "Ascii Sum Hash"},
    {HashNormalSum,   "Normilized Ascii Sum Hash"},
    {HashRotateRight, "ROR hash"},
    {HashRotateLeft,  "ROL hash"},
    {HashCrc,         "CRC32 hash"},
};

#endif
