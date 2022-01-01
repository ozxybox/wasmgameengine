#include "map.h"
#include "stdlib.h"
#include "string.h"

map_t map_create()
{
    return (map_t){0,0};
}

void map_clear(map_t* map)
{
    mapblock_t* n = 0;
    for(mapblock_t* b = map->first; b; b = n)
    {
        n = b->next;
        free(b);
    }
    map->current = 0;
    map->first = 0;
}

int map_find(map_t* restrict map, mapkey_t key, mapblock_t** oblock, int* opos)
{
    for(mapblock_t* b = map->first; b; b = b->next)
    {
        short freespace = b->freespace;
        for(int i = 0; i < MAP_PAIRS_PER_BLOCK; i++)
        {
            if(!(freespace & (1 << i)))
            {
                mappair_t* p = &b->pairs[i];

                // Need to compare the strings incase of collision
                if(p->key == key)
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

mappair_t* map_alloc(map_t* map)
{
    // Find open space
    
    // Where's an open block
    mapblock_t* block = map->current ? map->current : (map->first ? map->first : 0);

    if(block == 0 || block->freespace == MAP_FULL)
    {
        // Need a new block
        mapblock_t* nblk = (mapblock_t*)malloc(sizeof(mapblock_t));
        nblk->next = 0;
        nblk->freespace = MAP_EMPTY;
        
        if(block)
            block->next = nblk;
        else
            map->first = nblk;
        map->current = nblk;
        block = nblk;
    }

    // Find freespace
    short freespace = block->freespace;
    int open;
    for(int i = 0; i < MAP_PAIRS_PER_BLOCK; i++)
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

void map_set(map_t* map, mapkey_t key, void* value)
{
    // Is this in the map already?
    mapblock_t* block;
    int i;
    int found = map_find(map, key, &block, &i);

    // Where are we assigning?
    if(found)
    {
        // Somewhere we know 
        mappair_t* pair = &block->pairs[i];

        // Set our value
        pair->value = value;
    }
    else
    {
        // Somewhere new
        mappair_t* pair = map_alloc(map);
        
        // Pop it in place
        *pair = (mappair_t){key, value};
    }

}


int map_remove(map_t* map, mapkey_t key)
{
    mapblock_t* b;
    int i;
    if(map_find(map, key, &b, &i))
    {
        b->freespace &= ~(1 << i);
        b->pairs[i] = (mappair_t){0,0};

        // Reset our current back to start
        map->current = map->first;
        return 1;
    }
    return 0;
}

int map_get(map_t* map, mapkey_t key, void** out)
{
    mapblock_t* b;
    int i;
    if(map_find(map, key, &b, &i))
    {
        if(out)
            *out = b->pairs[i].value;
        return 1;
    }
    return 0;
}