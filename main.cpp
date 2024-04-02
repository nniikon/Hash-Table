#include <inttypes.h>

#include "./hash_table/hash_table.h"
#include "./logs/logs.h"
#include "./file_to_buffer/fileToBuffer.h"

const char gLogFileName[] = "./build/log_file.html";

uint64_t TestHashFunction(const void* mem, size_t len) {

    const char* ch_mem = (const char*)mem;

    uint64_t hash = 0;
    for (size_t i = 0; i < len; i++) {
        hash += (size_t)ch_mem[i] * i;
    }

    return hash;
}


int InsertDictionary(ht_HashTable* ht, const char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (file == nullptr) {
        return -1;
    }

    size_t size = 0;

    char* c_dict = (char*) ftbPutFileToBuffer(&size, file);
    if (c_dict == nullptr) {
        fclose(file);
        return -1;
    }

    size_t cur_pos = 0;

    while (cur_pos < size) {
        int num_len = 0; 
        size_t len = 0;
        const char* str = nullptr;

        sscanf(c_dict + cur_pos, "%lu$%n", &len, &num_len);
        cur_pos += (size_t)num_len;
        str = c_dict + cur_pos;
        cur_pos += len + 1;
        fprintf(stderr, "curpos:    %lu\n", cur_pos);
        fprintf(stderr, "len:       %lu\n", len);
        fprintf(stderr, "num_len:   %d \n", num_len);
        fprintf(stderr, "word:      %s \n", str);

        ht_Error err = ht_Insert(ht, str, len);
        if (err) {
            fclose(file);
            return -1;
        }

        ht_Dump(ht);
    }

    
    free(c_dict);
    fclose(file);
    return 0;
}


int main() {

    FILE* log_file = logOpenFile(gLogFileName);
    if (log_file == nullptr) {
        return 1;
    }

    ht_SetLogFile(log_file);

    ht_HashTable ht = {};

    ht_Error err = ht_Contructor(&ht, 257, TestHashFunction);
    if (err) {
        fclose(log_file);
        return 2;
    }

    const char* hui = "montaque";
    ht_Insert(&ht, hui, 8);
    ht_Insert(&ht, hui, 8);
    //InsertDictionary(&ht, "output.txt");

    ht_Dump(&ht);

    fclose(log_file);
    ht_Destructor(&ht);

    return 0;
}
