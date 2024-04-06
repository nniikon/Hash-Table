#include "hash_table.h"

#include <assert.h>
#include <stdlib.h>
#include "../logs/logs.h"

static FILE* gLogFile = nullptr;

// RETURN_AND_DUMP_ERROR();
// RETURN_ON_ERROR_WITH_DUMB();
#define DUMP_RETURN_ERROR(err)                                                 \
    do {                                                                       \
        LOGF_ERR(gLogFile, "%s\n", ht_GetErrorMsg(err));                       \
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

inline static List* ht_GetListByString(ht_HashTable* ht, const char* str,
                                size_t len, uint64_t* ret_hash, int* listIndex);


void ht_SetLogFile(FILE* log_file) {
    listSetLogFile(log_file);
    gLogFile = log_file;
}


inline static List* ht_GetListByString(ht_HashTable* ht, const char* str,
                               size_t len, uint64_t* ret_hash, int* listIndex) {

    uint64_t hash = ht->hash_function((const void*)str, len);

    int index = (int)(hash % ht->n_buckets);

    List* list = &ht->lists[index];

    // TODO: add error check
    if (listIndex) {
        listLookUp16(list, str, len, listIndex);
    }

    if (ret_hash) {
        *ret_hash = hash;
    }

    return list;
}


ht_Error ht_Remove(ht_HashTable* ht, const char* str, size_t len) {
    assert(ht);
    assert(str);

    int listIndex = 0;

    List* list = ht_GetListByString(ht, str, len, nullptr, &listIndex);

    // If the string is not in the list
    if (listIndex == -1) {
        return HT_ERR_NO_SUCH_ELEMENT;
    }

    DLL_Error err = listDelete(list, listIndex);
    if (err) {
        DUMP_RETURN_ERROR(HT_ERR_LIST);
    }

    return HT_ERR_NO;
}


ht_Error ht_LookUp(ht_HashTable* ht, const char* str, size_t len, size_t* value) {
    assert(ht);
    assert(str);

    int listIndex = 0;
    List* list = ht_GetListByString(ht, str, len, nullptr, &listIndex);

    // If the string is not in the list
    if (listIndex == -1) {
        *value = 0;
        return HT_ERR_NO_SUCH_ELEMENT;
    }

    *value = list->data[listIndex].occurrences;
    return HT_ERR_NO;
}


ht_Error ht_Insert(ht_HashTable* ht, const char* str, size_t len) {
    assert(ht);
    assert(str);

    uint64_t hash = 0;
    int listIndex = 0;
    List* list = ht_GetListByString(ht, str, len, &hash, &listIndex);

    // If the string is already in the list
    if (listIndex != -1) {
        list->data[listIndex].occurrences++;
        return HT_ERR_NO;
    }

    ht_ListElem listElem = {
        .str = str,
        .hash = hash,
        .occurrences = 1,
    };

    DLL_Error err = listPushFront(list, listElem);
    if (err) {
        DUMP_RETURN_ERROR(HT_ERR_LIST);
    }

    return HT_ERR_NO;
}


ht_Error ht_Contructor(ht_HashTable* ht, size_t n_buckets, 
                      uint64_t (*hash_function)(const void* mem, size_t size)) {
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


void ht_Dump_internal(ht_HashTable* ht) {
    assert(ht);

    LOG_START_COLOR(gLogFile, blue);

    fprintf(gLogFile, "\n================ HASH TABLE DUMP ================\n");

    for (size_t bucket = 0; bucket < ht->n_buckets; bucket++) {

        List list = ht->lists[bucket];
        int current_index = list.next[-1];

        fprintf(gLogFile, "\t bucket %lu: \t\t", bucket);

        while (current_index != -1) {
            fprintf(gLogFile, "%s (%lu) | ", list.data[current_index].str,
                                            list.data[current_index].occurrences);

            current_index = list.next[current_index];
        }

        fprintf(gLogFile, "\n");
    }

    fprintf(gLogFile, "\n=================================================\n");
    
    LOG_END(gLogFile);
}
