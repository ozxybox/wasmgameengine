#pragma once

#include "hash.h"

// MAX is how many bits are in hashmapblock_t::freespace
#define HASHMAP_PAIRS_PER_BLOCK 16
#define HASHMAP_FULL  0x0000
#define HASHMAP_EMPTY 0xFFFF

typedef struct hashmappair_t
{
    hash_t hash;
    const char* key;
    void* value;    
    /*const*/ int pos; // Pos of this pair in the map
} hashmappair_t;

typedef struct hashmapblock_t
{  
    /*const*/ int pos; // pos of this block in the chain
    struct hashmapblock_t* next;
    short freespace;
    hashmappair_t pairs[HASHMAP_PAIRS_PER_BLOCK];
} hashmapblock_t;

typedef struct hashmap_t
{
    hashmapblock_t* first;
    hashmapblock_t* current;

} hashmap_t;



hashmap_t hm_create();
void hm_clear(hashmap_t* map);

// Does not copy out the value. Do NOT delete KEY. THIS DOES NOT COPY
void hm_set(hashmap_t* map, const char* key, void* value);
// 1 on success
int hm_remove(hashmap_t* map, const char* key);
int hm_get(hashmap_t* map, const char* key, void** out);
int hm_getpair(hashmap_t* map, const char* key, hashmappair_t** out);

int hm_at(hashmap_t* map, unsigned int index, hashmappair_t** out);
int hm_indexof(hashmap_t* map, const char* key, unsigned int** out);
//int hm_indexofvalue(void* value);
