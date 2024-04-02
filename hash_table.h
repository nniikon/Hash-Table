#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include "./list/include/DLL.h"

#include <inttypes.h>
#include <stdio.h>

enum ht_Error
{
    #define DEF_HT_ERR(err, msg) HT_ERR_ ## err,

        #include "hash_table_errgen.inc"

    #undef  DEF_HT_ERR
};

struct ht_HashTable {
    uint64_t (*hash_function)(void* mem, size_t size); // expensive but beautiful
    size_t n_buckets;
    List* lists;
};

void     ht_SetLogFile(FILE* log_file);

ht_Error ht_LookUp    (ht_HashTable* ht, const char* str, size_t len, size_t* value);
ht_Error ht_Insert    (ht_HashTable* ht, const char* str, size_t len);
ht_Error ht_Destructor(ht_HashTable* ht);
ht_Error ht_Contructor(ht_HashTable* ht, size_t n_buckets, 
                             uint64_t (*hash_function)(void* mem, size_t size));

#endif
