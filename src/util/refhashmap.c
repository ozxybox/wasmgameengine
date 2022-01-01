#include "refhashmap.h"
#include <string.h>

// Need to pull these in from hashmap. They aren't in the header, as they're internal utils
extern hashmappair_t* hm_alloc(hashmap_t* map);
extern int hm_find(hashmap_t* restrict map, const char* restrict key, hashmapblock_t** oblock, int* opos, hash_t* ohash);


refhashmap_t rhm_create(unsigned int elementsize, reffunc_t deconstructor)
{
    return (refhashmap_t){hm_create(), rp_create(elementsize, deconstructor)};
}

void rhm_clear(refhashmap_t* map)
{
    hm_clear(&map->hashmap);
    rp_clear(&map->refpool);
}


void rhm_set(refhashmap_t* map, const char* key, void* value)
{
    // Is this in the map already?
    hashmapblock_t* block;
    int i;
    hash_t hash;
    int found = hm_find(&map->hashmap, key, &block, &i, &hash);

    if(found)
    {
        // Assign the old ref to the new memory
        hashmappair_t* pair = &block->pairs[i];
        memcpy(pair->value, value, map->refpool.elementsize);
    }
    else
    {
        // Push in a new ref
        hashmappair_t* pair = hm_alloc(&map);
        
        ref_t ref = rp_push(&map->refpool, value);
        *pair = (hashmappair_t){hash, key, ref};
    }
}

/*
int rhm_remove(refhashmap_t* map, const char* key)
{
}
*/

int rhm_get(refhashmap_t* map, const char* key, ref_t* out)
{
    return hm_get(&map->hashmap, key, (void**)out);
}

int rhm_inc(refhashmap_t* map, const char* key)
{
    ref_t ref;
    if(rhm_get(map, key, &ref))
    {
        ref_inc(ref);
        return 1;
    }
    return 0;
}
int rhm_dec(refhashmap_t* map, const char* key)
{
    ref_t ref;
    if(rhm_get(map, key, &ref))
    {
        ref_dec(ref);
        return 1;
    }
    return 0;
}