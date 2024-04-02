#include "../include/DLL.h"

#include "../../logs/logs.h"

#include <stdlib.h>


static FILE* logFile = stderr;


const char *const DLL_ERROR_MSG[] =
{
    #define DEF_ERR(err, str) str,
    #include "../include/DLL_errors_codegen.h"
    #undef DEF_ERR
};


#define DUMP_AND_RETURN_ERROR(err)                          \
    do                                                      \
    {                                                       \
        LOGF_ERR(logFile, "%s\n", DLL_ERROR_MSG[(int)err]); \
        return err;                                         \
    } while (0)                                             \


#ifndef NDEBUG 
    #define VERIFY_DUMP_RETURN_ERROR(list)              \
    do                                                  \
    {                                                   \
        DLL_Error err = listVerify(list);               \
        if (err != DLL_ERR_OK)                          \
        {                                               \
            DUMP_AND_RETURN_ERROR(err);                 \
        }                                               \
    } while (0)
#else
    #define VERIFY_DUMP_RETURN_ERROR(list) do {} while(0)
#endif

static bool isInCorrectRange(List* list, int var)
{
    if (var < -1 || var >= (int) list->listInfo.capacity)
        return false;
    return true;
}


DLL_Error listVerify(List* list)
{
    if (list == NULL)        DUMP_AND_RETURN_ERROR(DLL_ERR_NULL_LIST_PASSED);
    if (list->data == NULL)  DUMP_AND_RETURN_ERROR(DLL_ERR_NULL_DATA_ARRAY_PASSED);
    if (list->prev == NULL)  DUMP_AND_RETURN_ERROR(DLL_ERR_NULL_PREV_ARRAY_PASSED);
    if (list->next == NULL)  DUMP_AND_RETURN_ERROR(DLL_ERR_NULL_NEXT_ARRAY_PASSED);

    if (!isInCorrectRange(list, list->prev[-1]))
        return DLL_ERR_INVALID_HEAD_INDEX;

    if (!isInCorrectRange(list, list->next[-1]))
        return DLL_ERR_INVALID_TAIL_INDEX;

    if (!isInCorrectRange(list, list->free))
        return DLL_ERR_INVALID_FREE_INDEX;

    return DLL_ERR_OK;
}


static DLL_Error allocListMem(listElem** data, int** prev, int** next, unsigned int capacity)
{
    LOGF(logFile, "allocListMem() started\n");

    *data = (listElem*) calloc(sizeof(data[0]), capacity + 1);
    if (data == NULL)
    {
        DUMP_AND_RETURN_ERROR(DLL_ERR_MEMORY_ALLOCATION_FAILURE);
    }

    *prev = (int*) calloc(sizeof(prev[0]), capacity + 1);
    if (prev == NULL)
    {
        free(data);
        DUMP_AND_RETURN_ERROR(DLL_ERR_MEMORY_ALLOCATION_FAILURE);
    }

    *next = (int*) calloc(sizeof(next[0]), capacity + 1);
    if (next == NULL)
    {
        free(data);
        free(prev);
        DUMP_AND_RETURN_ERROR(DLL_ERR_MEMORY_ALLOCATION_FAILURE);
    }

    (*data)++;
    (*prev)++;
    (*next)++;

    return DLL_ERR_OK;
}


void listSetLogFile(FILE* file) 
{
    logFile = file;
}


DLL_Error listConstuctor(List* list)
{
    LOGF(logFile, "listConstuctor() started.\n");

    if (list == NULL)
        DUMP_AND_RETURN_ERROR(DLL_ERR_NULL_LIST_PASSED);

    list->logFile = logFile;

    if (allocListMem(&list->data, &list->prev, &list->next, DLL_DEFAULT_CAPACITY) != DLL_ERR_OK)
        return DLL_ERR_MEMORY_ALLOCATION_FAILURE;

    // Fill in arrays with info.
    for (unsigned int i = 0; i < DLL_DEFAULT_CAPACITY; i++)
    {
        list->prev[i] = DLL_PREV_POISON;
        list->next[i] = (int) i + 1;
    }
    // Loop the next[] array.
    list->next[DLL_DEFAULT_CAPACITY - 1] = -1; 

    list->free = 0;
    list->prev[-1] = -1;
    list->next[-1] = -1;
    list->listInfo.capacity = DLL_DEFAULT_CAPACITY;
    list->listInfo.size     = 0;
    list->listInfo.isSorted = true;

    LOGF(logFile, "listConstuctor() success.\n");
    return DLL_ERR_OK;
}


DLL_Error listDestructor(List* list)
{
    LOGF(logFile, "listDestructor() started.\n");
    if (list == NULL) 
        DUMP_AND_RETURN_ERROR(DLL_ERR_NULL_LIST_PASSED);

    if (list->next != NULL)
        free(list->next - 1);

    if (list->prev != NULL)
        free(list->prev - 1);

    if (list->data != NULL)
        free(list->data - 1);

    LOGF(logFile, "listDestructor() success.\n");

    return DLL_ERR_OK;
}


DLL_Error listDelete(List* list, int index)
{
    LOGF(logFile, "listDelete(%d) started.\n", index);
    if (list == NULL)
        DUMP_AND_RETURN_ERROR(DLL_ERR_NULL_LIST_PASSED);
    if (index <= 0)
        DUMP_AND_RETURN_ERROR(DLL_ERR_INVALID_INDEX_PASSED);
    VERIFY_DUMP_RETURN_ERROR(list);

    int indPrev = list->prev[index];
    int indNext = list->next[index];

    list->prev[index] = DLL_PREV_POISON;
    list->next[index] = list->free;

    list->free = index;

    list->prev[indNext] = indPrev;
    list->next[indPrev] = indNext;

    list->listInfo.size--;

    return DLL_ERR_OK;
}


DLL_Error listInsertAfter(List* list, int index, listElem value)
{
    LOGF(logFile, "listInsertAfter(%d, value) started.\n", index);
    if (list == NULL)
        DUMP_AND_RETURN_ERROR(DLL_ERR_NULL_LIST_PASSED);
    VERIFY_DUMP_RETURN_ERROR(list);

    if (list->free >= (int) list->listInfo.capacity - 1)
        listChangeCapacity(list, DLL_CAPACITY_MULTIPLIER);

    int freeIndex = list->free;

    list->free = list->next[list->free];
    list->listInfo.size++;

    list->data[freeIndex] = value;
    list->prev[list->next[index]] = freeIndex;
    list->next[freeIndex] = list->next[index];
    list->prev[freeIndex] = index;

    list->next[index] = freeIndex;

    if (index == list->prev[-1])
    {
        list->prev[-1] = freeIndex;
        LOGF_COLOR(logFile, green, "TAIL: %d\n", list->prev[-1]);
    }

    return DLL_ERR_OK;
}


DLL_Error listInsertBefore(List* list, int index, listElem value)
{
    LOGF(logFile, "listInsertAfter(%d, value) started.\n", index);
    if (list == NULL)
        DUMP_AND_RETURN_ERROR(DLL_ERR_NULL_LIST_PASSED);
    VERIFY_DUMP_RETURN_ERROR(list);

    if (list->prev[index] == DLL_PREV_POISON)
    {
        DUMP_AND_RETURN_ERROR(DLL_ERR_ELEMENT_DOESNT_EXIST);
        return DLL_ERR_ELEMENT_DOESNT_EXIST;
    }
    return listInsertAfter(list, list->prev[index], value);
}


DLL_Error listPushFront(List* list, listElem value)
{
    LOGF(logFile, "listPushFront() started.\n");
    if (list == NULL)
        DUMP_AND_RETURN_ERROR(DLL_ERR_NULL_LIST_PASSED);
    VERIFY_DUMP_RETURN_ERROR(list);

    return listInsertAfter(list, -1, value);
}


DLL_Error listPushBack(List* list, listElem value)
{
    LOGF(logFile, "listPushFront() started.\n");
    if (list == NULL)
        DUMP_AND_RETURN_ERROR(DLL_ERR_NULL_LIST_PASSED);
    VERIFY_DUMP_RETURN_ERROR(list);

    return listInsertBefore(list, -1, value);
}


/*static*/ DLL_Error listChangeCapacity(List* list, float multiplier)
{
    LOGF(logFile, "listChangeCapacity(%f) started.\n", multiplier);

    unsigned int newCapacity = (unsigned int)((float) list->listInfo.capacity * multiplier);

    listElem* tempData = (listElem*) realloc(list->data - 1, sizeof(listElem) * (newCapacity + 1));
    if (tempData == NULL)
    {
        DUMP_AND_RETURN_ERROR(DLL_ERR_MEMORY_ALLOCATION_FAILURE);
    }

    int* tempNext = (int*) realloc(list->next - 1, sizeof(int) * (newCapacity + 1));
    if (tempData == NULL)
    {
        free(tempData);
        DUMP_AND_RETURN_ERROR(DLL_ERR_MEMORY_ALLOCATION_FAILURE);
    }

    int* tempPrev = (int*) realloc(list->prev - 1, sizeof(int) * (newCapacity + 1));
    if (tempPrev == NULL)
    {
        free(tempData);
        free(tempNext);
        DUMP_AND_RETURN_ERROR(DLL_ERR_MEMORY_ALLOCATION_FAILURE);
    }

    list->data = tempData + 1;
    list->next = tempNext + 1;
    list->prev = tempPrev + 1;

    // Fill in arrays with info.
    for (size_t i = list->listInfo.capacity - 1; i < newCapacity; i++)
    {
        list->prev[i] = DLL_PREV_POISON;
        list->next[i] = (int) i + 1;
    }

    list->next[newCapacity - 1] = -1;

    list->listInfo.capacity = newCapacity;

    LOGF(logFile, "listChangeCapacity() success.\n");
    return DLL_ERR_OK;
}


DLL_Error listLinearize(List* list)
{
    LOGF(logFile, "listLinearize() started.\n");

    listElem* newData = NULL;
    int*    newPrev = NULL;
    int*    newNext = NULL;

    if (allocListMem(&newData, &newPrev, &newNext, list->listInfo.capacity) != DLL_ERR_OK)
        DUMP_AND_RETURN_ERROR(DLL_ERR_MEMORY_ALLOCATION_FAILURE);

    int oldIndex = list->next[-1];
    size_t newIndex = 0;
    while (list->next[oldIndex] != -1)
    {
        newData[newIndex] = list->data[oldIndex];
        newIndex++;
        oldIndex = list->next[oldIndex];
    }

    for (size_t i = 0; i < newIndex + 1; i++)
    {
        newNext[i] = (int) i + 1;
        newPrev[i] = (int) i - 1;
    }
    for (size_t i = newIndex + 1; i < list->listInfo.capacity; i++)
    {
        newNext[i] = (int) i + 1;
        newPrev[i] = DLL_PREV_POISON;
    }

    free(list->data - 1);
    free(list->next - 1);
    free(list->prev - 1);

    list->data = newData;
    list->next = newNext;
    list->prev = newPrev;

    list->next[-1] = 0;
    list->prev[-1] = (int) newIndex;

    list->next[newIndex] = -1;
    list->next[list->listInfo.capacity - 1] = -1;

    return DLL_ERR_OK;
}


DLL_Error listLookUp(List* list, listElem value)
{
    LOGF(logFile, "listLookUp() started.\n");

    size_t index = list->next[-1];

    while (list->next[index] != -1) 
    {
        
        index = list->next[index];
    }
    

    return DLL_ERR_OK;
}

