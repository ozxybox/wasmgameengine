#pragma once

#include <stddef.h>
typedef size_t hash_t;

hash_t hashBufferCaseInvariant(const char* restrict buffer, size_t length);
hash_t hashBuffer(const char* restrict buffer, size_t length);
hash_t hashStringCaseInvariant(const char* restrict string);
hash_t hashString(const char* restrict string);