#pragma once

#include <stddef.h>

// MAX is how many bits are in block_t::freespace
#define MAP_PAIRS_PER_BLOCK 16
#define MAP_FULL  0x0000
#define MAP_EMPTY 0xFFFF

typedef size_t mapkey_t;

typedef struct mappair_t
{
    mapkey_t key;
    void* value;
} mappair_t;

typedef struct mapblock_t
{  
    struct mapblock_t* next;
    short freespace;
    mappair_t pairs[MAP_PAIRS_PER_BLOCK];
} mapblock_t;

typedef struct map_t
{
    mapblock_t* first;
    mapblock_t* current;
} map_t;


map_t map_create();
void map_clear(map_t* map);

// THIS DOES NOT COPY
void map_set(map_t* map, mapkey_t key, void* value);
// 1 on success
int map_remove(map_t* map, mapkey_t key);
int map_get(map_t* map, mapkey_t key, void** out);