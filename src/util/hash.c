#include "hash.h"

#include <string.h>
#include <ctype.h>


// FNV-1a hash

// We've got diffent values on different platforms
#if __POINTER_WIDTH__ == 64 //_WIN64 || __x86_64__ || __ppc64__
#define HASH_BASIS 0xcbf29ce484222325
#define HASH_PRIME 0x100000001b3
#else
#define HASH_BASIS 0x811c9dc5
#define HASH_PRIME 0x01000193
#endif

hash_t hashBufferCaseInvariant(const char* restrict buffer, size_t length)
{
    // Hash of all chars in the buffer, turned uppercase
    hash_t hash = HASH_BASIS;
    for (size_t i = 0; i < length; i++)
        hash = (hash ^ toupper(buffer[i])) * HASH_PRIME;
    return hash;
}

hash_t hashBuffer(const char* restrict buffer, size_t length)
{
    // Hash of all chars in the buffer
    hash_t hash = HASH_BASIS;
    for (size_t i = 0; i < length; i++)
        hash = (hash ^ (buffer[i])) * HASH_PRIME;
    return hash;
}

hash_t hashStringCaseInvariant(const char* restrict string)
{
    // Hash of all chars in the buffer, turned uppercase
    hash_t hash = HASH_BASIS;
    for (;*string; string++)
        hash = (hash ^ toupper(*string)) * HASH_PRIME;
    return hash;
}

#include "log.h"

hash_t hashString(const char* restrict string)
{
    // Hash of all chars in the buffer, turned uppercase
    hash_t hash = HASH_BASIS;
    for (; *string; string++)
        hash = (hash ^ (*string)) * HASH_PRIME;
    return hash;
}