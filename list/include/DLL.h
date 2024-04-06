#ifndef DLL_HD5H3KR6V4_H
#define DLL_HD5H3KR6V4_H

#include <stdio.h>
#include <inttypes.h>

#include "DLL_config.h"

enum DLL_Error 
{
    #define DEF_ERR(err, ...) DLL_ERR_ ## err,
    #include "DLL_errors_codegen.h"
    #undef DEF_ERR
};

typedef int ht_valueElem;

struct ht_ListElem
{
    const char*    str;
    uint64_t      hash;
    size_t occurrences;
};

typedef ht_ListElem listElem; // FIXME: cringe obv

struct DLL_ListInfo
{
    unsigned int capacity; 
    int size;
    bool isSorted;
};

struct List
{
    listElem* data;
    int* prev;
    int* next;
    int free;
    DLL_ListInfo listInfo;
    FILE* logFile;
};

void      listSetLogFile    (FILE* file);
DLL_Error listConstuctor    (List* list);
DLL_Error listDestructor    (List* list);
DLL_Error listVerify        (List* list);
DLL_Error listDelete        (List* list, int index);
DLL_Error listInsertAfter   (List* list, int index, listElem value);
DLL_Error listInsertBefore  (List* list, int index, listElem value);
DLL_Error listPushFront     (List* list, listElem value);
DLL_Error listPushBack      (List* list, listElem value);
DLL_Error listChangeCapacity(List* list, float multiplier);
DLL_Error listLinearize     (List* list);
DLL_Error listLookUp        (List* list, const char* str, size_t len, int* value);
DLL_Error listLookUp16      (List* list, const char* str, size_t len, int* value);

#endif
