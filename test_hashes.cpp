#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "./hash_table/hash_table.h"
#include "./logs/logs.h"
#include "./file_to_buffer/fileToBuffer.h"
#include "./hash_functions/hash_functions.h"


const char gLogFileName[] = "./build/log_file.html";
const char gDictName[]    = "dict.txt";
#define RESULT_NAME_MASK "new_result%d.txt"


int InsertDictionary (ht_HashTable* ht, const char* c_dict, size_t size);
int GetResults       (ht_HashTable* ht, const char* hist_name);
int TestHashFunctions(ht_HashTable* ht, const char* dict, size_t size,
                      const HashFunction* hash_functions, size_t n_funcs);


int main() {
    FILE* log_file = nullptr;
    int ret_value = 0;
    size_t dict_size = 0;
    char* c_dict = nullptr;
    ht_HashTable ht = {};
    ht_Error err = HT_ERR_NO;

    FILE* file = fopen(gDictName, "r");
    if (file == nullptr) {
        ret_value = -1;
        goto fail_file;
    }

    c_dict = (char*) ftbTransferBufferTo16(&dict_size, file);
    if (c_dict == nullptr) {
        ret_value = -1;
        goto fail_dict;
    }

    log_file = logOpenFile(gLogFileName);
    if (log_file == nullptr) {
        ret_value = -1;
        goto fail_logfile;
    }

    ht_SetLogFile(log_file);

    TestHashFunctions(&ht, c_dict, dict_size, 
              gHashFunctions, sizeof(gHashFunctions)/sizeof(gHashFunctions[0]));

    fclose(log_file);
fail_logfile:
    free(c_dict);
fail_dict:
    fclose(file);
fail_file:
    return ret_value;
}


// TODO: add error check
int TestHashFunctions(ht_HashTable* ht, const char* dict, size_t size,
                      const HashFunction* hash_functions, size_t n_funcs) {

    for (size_t i = 0; i < n_funcs; i++) {
        ht_Contructor   (ht, 997, hash_functions[i].hash_func);
        InsertDictionary(ht, dict, size);
        GetResults      (ht, hash_functions[i].description);
        ht_Destructor   (ht);
    }

    return 0;
}


int InsertDictionary(ht_HashTable* ht, const char* c_dict, size_t size) {
    const char* str = c_dict;

    while (str < c_dict + size) {
        size_t len = strlen(str);

        ht_Error err = ht_Insert(ht, str, len);
        if (err) {
            return -1;
        }

        str += ht_gMaxWordLen;

        //ht_Dump_internal(ht);
    }
    
    return 0;
}


int GetResults(ht_HashTable* ht, const char* hist_name) {
    static int result_index = 0;
    const size_t file_name_len = sizeof(RESULT_NAME_MASK) + 128;
    const size_t command_len = 256;

    char file_name[file_name_len] = {};
    char command[command_len] = {};

    snprintf(file_name, file_name_len, RESULT_NAME_MASK, result_index++);

    FILE* file = fopen(file_name, "w");
    if (file == nullptr) {
        return -1;
    }

    size_t size = ht->n_buckets;

    for (int i = 0; i < size; i++) {
        fprintf(file, "%d %d\n", i, ht->lists[i].listInfo.size);   
    }

    fclose(file);

    snprintf(command, command_len, "python3 scripts/histogram.py %s %s", file_name, hist_name);
    system(command);

    //snprintf(command, command_len, "rm %s", file_name);
    //system(command);

    return 0;
}
