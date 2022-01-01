#include "hashmap.h"
#include "stdlib.h"
#include "string.h"

// I would slap hashmap and map together, but it doesn't seem worth it, as hashmap needs a lot extra

#define HASH_FUNC hashString

hashmap_t hm_create()
{
    return (hashmap_t){0,0};
}
void hm_clear(hashmap_t* map)
{
    hashmapblock_t* n = 0;
    for(hashmapblock_t* b = map->first; b; b = n)
    {
        n = b->next;
        free(b);
    }
    map->current = 0;
    map->first = 0;
}

int hm_find(hashmap_t* restrict map, const char* restrict key, hashmapblock_t** oblock, int* opos, hash_t* ohash)
{
    hash_t hash = HASH_FUNC(key);
    if(ohash) *ohash = hash;
    for(hashmapblock_t* b = map->first; b; b = b->next)
    {
        short freespace = b->freespace;
        for(int i = 0; i < HASHMAP_PAIRS_PER_BLOCK; i++)
        {
            if(!(freespace & (1 << i)))
            {
                hashmappair_t* p = &b->pairs[i];

                // Need to compare the strings incase of collision
                if(p->hash == hash && !strcmp(key, p->key))
                {
                    *oblock = b;
                    *opos = i;
                    return 1;
                }
            }
        }
    }
    return 0;
}

hashmappair_t* hm_alloc(hashmap_t* map)
{
    // Find open space
    
    // Where's an open block
    hashmapblock_t* block = map->current ? map->current : (map->first ? map->first : 0);

    if(block == 0 || block->freespace == HASHMAP_FULL)
    {
        // Need a new block
        hashmapblock_t* nblk = (hashmapblock_t*)malloc(sizeof(hashmapblock_t));
        nblk->next = 0;
        nblk->freespace = HASHMAP_EMPTY;
        
        if(block)
        {
            block->next = nblk;
            nblk->pos = block->pos + HASHMAP_PAIRS_PER_BLOCK;
        }
        else
        {
            map->first = nblk;
            nblk->pos = 0;
        }
        map->current = nblk;
        block = nblk;

        // Set up pair positions
        for(int i = 0; i < HASHMAP_PAIRS_PER_BLOCK; i++)
            block->pairs[i].pos = block->pos + i;
    }

    // Find freespace
    short freespace = block->freespace;
    int open;
    for(int i = 0; i < HASHMAP_PAIRS_PER_BLOCK; i++)
    {
        if(freespace & (1 << i))
        {
            // Free!
            open = i;
            break;
        }
    }

    // We checked earlier, so this block *should* have freespace, and we *shouldn't* have to check open
    // Mark we're going to use this space
    block->freespace &= ~(1 << open);

    return &block->pairs[open];
}

void hm_set(hashmap_t* map, const char* key, void* value)
{
    // Is this in the map already?
    hashmapblock_t* block;
    int i;
    hash_t hash;
    int found = hm_find(map, key, &block, &i, &hash);

    // Where are we assigning?
    if(found)
    {
        // Somewhere we know
        hashmappair_t* pair = &block->pairs[i]; 

        // Pop in our value
        pair->value = value;
    }
    else
    {
        // Somewhere new
        hashmappair_t* pair = hm_alloc(map); 

        // Pop it in place
        pair->hash = hash;
        pair->key = key;
        pair->value = value;
    }
}


int hm_remove(hashmap_t* map, const char* key)
{
    hashmapblock_t* b;
    int i;
    if(hm_find(map, key, &b, &i, 0))
    {
        b->freespace &= ~(1 << i);
        b->pairs[i] = (hashmappair_t){0,0};

        // Reset our current back to start
        map->current = map->first;
        return 1;
    }
    return 0;
}

int hm_get(hashmap_t* map, const char* key, void** out)
{
    hashmapblock_t* b;
    int i;
    if(hm_find(map, key, &b, &i, 0))
    {
        if(out)
            *out = b->pairs[i].value;
        return 1;
    }
    return 0;
}

int hm_getpair(hashmap_t* map, const char* key, hashmappair_t** out)
{
    hashmapblock_t* b;
    int i;
    if(hm_find(map, key, &b, &i, 0))
    {
        if(out)
            *out = &b->pairs[i];
        return 1;
    }
    return 0;
}


int hm_at(hashmap_t* map, unsigned int index, hashmappair_t** out)
{
    unsigned int block = index / HASHMAP_PAIRS_PER_BLOCK;
    unsigned int slot = index % HASHMAP_PAIRS_PER_BLOCK;

    // Seek out the block
    unsigned int at = 0;
    hashmapblock_t* b = map->first;
    for(; b && at != block; b = b->next)
        at++;

    // Could we seek?
    if(at != block || !b)
        return 0;

    // Is there something at this slot?
    short freespace = b->freespace;
    if(!(freespace & (1 << slot)))
    {
        if(out) 
            *out = &b->pairs[slot];
        return 1;
    }
    
    return 0;
}

int hm_indexof(hashmap_t* map, const char* key, unsigned int** out)
{
    hashmapblock_t* b;
    unsigned int p;
    if(hm_find(&map, key, &b, &p, 0))
    {
        if(out) *out = b->pairs[p].pos;
        return 1;
    }
    return 0;
}

/*
int hm_indexofvalue(void* value)
{
    // This is the start of the value. Let's walk it back into the pair
    hashmappair_t* pair = (char*)value - offsetof(hashmappair_t, value);
    return pair->pos;
}
*/

/*
void testHashmap()
{
    hashmap_t map = hm_create();

    char* s = malloc(5 * 256);
    char* k = s;

    for(int i = 0; i < 128; i++)
    {
        char* key = s;
        char* val = s+5;
        s+=10;
        snprintf(key, 5, "%d",  i);
        snprintf(val, 5, "%d", 20-i);
        hm_set(&map, key, val);
    }

    for(int i = 0; i < 128; i++)
    {
        char* val;
        hm_get(&map, k, &val);
        logInfo("%s = %s", k, val);
        k+=10;
    }
}
*/