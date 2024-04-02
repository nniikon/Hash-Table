#ifndef DLL_CONFIG_H
#define DLL_CONFIG_H

const unsigned int DLL_DEFAULT_CAPACITY = 8;

const float DLL_CAPACITY_MULTIPLIER = 2.0f;

const int DLL_PREV_POISON = -666;
const char* const DLL_DATA_POISON = (const char*) 0; 

#define LIST_RELEASE 

#endif
