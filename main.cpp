#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

#include "./hash_table/hash_table.h"
#include "./logs/logs.h"
#include "./file_to_buffer/fileToBuffer.h"
#include "./hash_functions/hash_functions.h"


const char gLogFileName[]    = "./build/log_file.html";
const char gDictName[]       = "dict.txt";

int InsertDictionary (ht_HashTable* ht, const char* c_dict, size_t size);
int TestLookUp       (ht_HashTable* ht, const char* file_name);

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

    err = ht_Contructor(&ht, 100000, HashCRC32_inline);
    if (err) {
        ret_value = -1;
        goto fail_constructor;
    }

    if (InsertDictionary(&ht, c_dict, dict_size)) {
        ret_value = -1;
        goto fail_insert;
    }

    if (TestLookUp(&ht, "dict.txt"))
    {
        ret_value = -1;
        goto fail_insert;
    }

fail_insert:
    ht_Destructor(&ht);
fail_constructor:
    fclose(log_file);
fail_logfile:
    free(c_dict);
fail_dict:
    fclose(file);
fail_file:
    return ret_value;
}


int TestLookUp(ht_HashTable* ht, const char* file_name) {
    if (!ht)
        return -1;

    FILE* lookup_file = fopen(file_name, "r");
    if (lookup_file == nullptr) {
        return -1;
    }

    size_t lookup_size = 0;
    char* c_lookup = (char*)ftbPutFileToBuffer(&lookup_size, lookup_file);
    if (c_lookup == nullptr) {
        fclose(lookup_file);
        return -1;
    }

    uint64_t start_time = __rdtsc();

    for (int i = 0; i < 200000; i++) {
        size_t cur_pos = 0;
        while (cur_pos < lookup_size) {
            const char* word = c_lookup + cur_pos;
            size_t len = strlen(word);

            size_t value = 0;
            //ht_LookUp(ht, word, len, &value);
            HashCRC32_inline(word, len);

            //fprintf(stderr, "word: %s (%lu)\n", word, value);

            cur_pos += len + 1;
        }
    }

    uint64_t end_time = __rdtsc();

    fprintf(stderr, "Time taken: %lg\n", (double)(end_time - start_time)/1e10);
    
    fclose(lookup_file);
    free(c_lookup);

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
    }
    
    return 0;
}
