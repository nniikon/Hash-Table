#ifndef HASH_FUNCTIONS_H_
#define HASH_FUNCTIONS_H_

#include <stdlib.h>
#include <inttypes.h>

struct HashFunction {
    uint64_t (*hash_func)(const void* mem, size_t size); // typedef
    const char* description;
};

uint64_t HashZero       (const void* mem, size_t size);
uint64_t HashLength     (const void* mem, size_t size);
uint64_t HashLetter     (const void* mem, size_t size);
uint64_t HashSum        (const void* mem, size_t size);
uint64_t HashNormalSum  (const void* mem, size_t size);
uint64_t HashRotateRight(const void* mem, size_t size);
uint64_t HashRotateLeft (const void* mem, size_t size);
uint64_t HashCRC32      (const void* mem, size_t size);
uint64_t HashKR         (const void* mem, size_t size);
uint64_t HashJenkins    (const void* mem, size_t size);
uint64_t HashElf        (const void* mem, size_t size);

const HashFunction gHashFunctions[] = 
{
    {HashZero,        "Zero Hash"},
    {HashSum,         "Ascii Sum Hash"},
    {HashNormalSum,   "Normilized Ascii Sum Hash"},
    {HashLetter,      "First letter Hash"},
    {HashLength,      "Length Hash"},
    {HashRotateLeft,  "ROL hash"},
    {HashRotateRight, "ROR hash"},
    {HashJenkins,     "Jenkins Hash"},
    {HashElf,         "Elf Hash"},
    {HashCRC32,       "CRC32 hash"},
    {HashKR,          "KR Hash"},
};

#endif
