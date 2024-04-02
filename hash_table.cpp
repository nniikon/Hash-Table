#include "hash_table.h"

#include <assert.h>
#include <stdlib.h>
#include "./logs/logs.h"

static FILE* kLogFile = nullptr;

#define DUMP_RETURN_ERROR(err)                                                 \
    do {                                                                       \
        LOGF_ERR(kLogFile, "%s\n", ht_GetErrorMsg(err));                       \
        return err;                                                            \
    } while(0)

const char* ht_GetErrorMsg(ht_Error err) {
    #define DEF_HT_ERR(err, msg)                                               \
        case HT_ERR_ ## err:                                                   \
            return msg;

    switch (err) {
        #include "hash_table_errgen.inc"
        default:
            return "No such error was found";
    }
    #undef DEF_HT_ERR
}


void ht_SetLogFile(FILE* log_file) {
    listSetLogFile(log_file);
    kLogFile = log_file;
}


ht_Error ht_Insert(ht_HashTable* ht, const char* str, size_t len) {
    assert(ht);
    assert(str);

    uint64_t hash = ht->hash_function((void*)str, len);

    size_t index = hash % ht->n_buckets;

    List list = ht->lists[index];

    size_t foundIndex = 0;
    DLL_Error err = listLookUp(&list, str, &foundIndex);
    if (err) {
        DUMP_RETURN_ERROR(HT_ERR_LIST);
    }

    // If the string is already in the list
    if (foundIndex != -1) {
        list.data[foundIndex].occurrences++;
        return HT_ERR_NO;
    }

    ht_ListElem listElem = {
        .str = str,
        .hash = hash,
        .occurrences = 1,
    };

    err = listPushFront(&list, listElem);
    if (err) {
        DUMP_RETURN_ERROR(HT_ERR_LIST);
    }

    return HT_ERR_NO;
}


ht_Error ht_Contructor(ht_HashTable* ht, size_t n_buckets, 
                            uint64_t (*hash_function)(void* mem, size_t size)) {
    assert(ht);
    assert(n_buckets > 0);


    List* lists = (List*) calloc(n_buckets, sizeof(List));
    if (lists == nullptr) {
        DUMP_RETURN_ERROR(HT_ERR_LIST);
    }

    for (int i = 0; i < n_buckets; i++) {
        DLL_Error error = listConstuctor(&lists[i]);
        if (error) {
            DUMP_RETURN_ERROR(HT_ERR_LIST);
        }
    }

    ht->lists = lists;
    ht->n_buckets = n_buckets;
    ht->hash_function = hash_function;

    return HT_ERR_NO;
}


ht_Error ht_Destructor(ht_HashTable* ht) {
    assert(ht);

    for (int i = 0; i < ht->n_buckets; i++) {
        listDestructor(&ht->lists[i]);
    }
    
    free(ht->lists);

    return HT_ERR_NO;
}
