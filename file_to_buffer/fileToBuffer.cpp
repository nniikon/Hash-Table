#include "fileToBuffer.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


static ssize_t getFileSize(FILE* file)
{
    struct stat bf = {};
    if (fstat(fileno(file), &bf) == -1)
       return -1;

    return (ssize_t)bf.st_size;
}


static char* putFileToBuffer(const size_t size, FILE* file)
{
    // Size is in BYTES (not the amount of 'char's).
    char* buffer = (char*) calloc(size + 2, 1);
    if (buffer == nullptr)
        return nullptr;

    size_t sizeRef = fread(buffer, 1, size, file);
    if (sizeRef != size)
    {
        free(buffer);
        return nullptr;
    }

    buffer[size] = '\0';
    buffer[size + 1] = '\0';

    return buffer;
}


void* ftbPutFileToBuffer(size_t* size, FILE* file)
{
    ssize_t bufferSize = getFileSize(file);
    if (bufferSize == -1)
        return nullptr;

    char* buffer = putFileToBuffer(bufferSize, file);

    if (buffer == nullptr)
        return nullptr;
    buffer[bufferSize] = '\0';

    if (size != nullptr)
        *size = bufferSize;

    return buffer;
}
