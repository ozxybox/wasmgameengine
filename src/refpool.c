#include "refpool.h"
#include "stdlib.h"
#include "string.h"
#include "log.h"

refpool_t rp_create(unsigned int elementsize, reffunc_t deconstructor)
{
    return (refpool_t){
        .first = 0,
        .current = 0,
        .elementsize = elementsize,
        .deconstructor = deconstructor,
    };
}

refpool_t* rp_createNew(unsigned int elementsize, reffunc_t deconstructor)
{
    refpool_t* pool = (refpool_t*)malloc(sizeof(refpool_t));
    *pool = rp_create(elementsize, deconstructor);
    return pool;
}


void rp_clear(refpool_t* refpool)
{
    refblock_t* n = 0;
    // Scan all blocks
    for(refblock_t* block = refpool->first; block; block = n)
    {
        n = block->next;

        // Run the deconstructor on em all
        char* data = (char*)(block) + sizeof(refblock_t);
        for(int i = 0; i < REF_PER_BLOCK; i++)
        {
            refcount_t* ref = (refcount_t*)(data + (sizeof(refcount_t) + refpool->elementsize) * i);
            
            if(*ref != REF_UNASSIGNED)
            {
                // This one's assigned!
                refpool->deconstructor((char*)ref + sizeof(refcount_t));
            }
        }

        // Free the whole block
        free(block);
    }
    refpool->current = 0;
    refpool->first = 0;
}

void rp_collect(refpool_t* refpool)
{
    refblock_t* block = refpool->first;
    // Scan all blocks
    while(block)
    {
        char* data = (char*)(block) + sizeof(refblock_t);
        
        // Look for 0 ref mem
        for(int i = 0; i < REF_PER_BLOCK; i++)
        {
            char* ref = data + (sizeof(refcount_t) + refpool->elementsize) * i;

            if(*(refcount_t*)ref == 0)
            {
                // Assigned and 0 refs
                void* mem = sizeof(refcount_t) + ref;
                
                // Deconstruct it if we have a function for it
                if(refpool->deconstructor)
                    refpool->deconstructor(mem);

                // Mark free
                *(refcount_t*)ref = REF_UNASSIGNED;
            }
        }

        // Next block
        block = block->next;
    }

}

ref_t rp_push(refpool_t* refpool, void* mem)
{
    refidx_t idx = REF_INVALID;
    refblock_t* last = 0;
    refblock_t* block = refpool->current ? refpool->current : refpool->first;

    // Scan each link
    char* data = (char*)(block) + sizeof(refblock_t);
    char* ref = 0;
    while(block)
    {
        // Find an open spot
        for(int i = 0; i < REF_PER_BLOCK; i++)
        {
            ref = data + (sizeof(refcount_t) + refpool->elementsize) * i;
            if(*(refcount_t*)ref == REF_UNASSIGNED)
            {
                // Open!
                idx = i;
                goto endLoop;
            }
        }

        // Next block?
        last = block;
        block = block->next;
    }
endLoop:

    // Did we find something?
    if(idx == REF_INVALID)
    {
        // All full? Allocate a new block
        refblock_t* nb = (refblock_t*)malloc(sizeof(refblock_t) + (sizeof(refcount_t) + refpool->elementsize) * REF_PER_BLOCK);
        nb->next = 0;

        // Set all as unassigned
        memset((char*)nb + sizeof(refblock_t), 0xFF, (refpool->elementsize + sizeof(refcount_t)) * REF_PER_BLOCK);

        // Link it
        if(block) block->next = nb;
        else if(last) last->next = nb;
        else if(!refpool->first) refpool->first = nb;
        else { /*what*/ }


        // Set it 
        block = nb;
        idx = 0;
        ref = (char*)nb + sizeof(refblock_t);
    }

    // Everything else is full, so use this one next time
    refpool->current = block; 

    // Pick it
    *(refcount_t*)ref = (refcount_t)0;

    // Copy in the value
    void* dst = ref + sizeof(refcount_t);
    memcpy(dst, mem, refpool->elementsize);

    return dst;
}

void ref_inc(ref_t ref)
{
    refcount_t* pCount = (char*)ref - sizeof(refcount_t);
    if(!ref || *pCount == REF_UNASSIGNED)
    {
        logError("Failed to incremented the ref counter of an unassigned reference!");
        return;
    }
    if(*pCount == REF_IMMORTAL)
    {
        logError("Failed to incremented the ref counter of an immortal ref!");
        return;
    }
    (*pCount)++;
}
void ref_dec(ref_t ref)
{
    refcount_t* pCount = (char*)ref - sizeof(refcount_t);
    if(!ref || *pCount == REF_UNASSIGNED)
    {
        logError("Failed to decremented the ref counter of an unassigned reference!");
        return;
    }
    if(*pCount == REF_IMMORTAL)
    {
        logError("Failed to decremented the ref counter of an immortal ref!");
        return;
    }
    (*pCount)--;
}

void ref_set(ref_t ref, refcount_t count)
{
    refcount_t* pCount = (char*)ref - sizeof(refcount_t);
    if(!ref || *pCount == REF_UNASSIGNED)
    {
        logError("Failed to set the ref counter of an unassigned reference!");
        return;
    }
    *pCount = count;
}

refcount_t ref_get(ref_t ref)
{
    refcount_t* pCount = (char*)ref - sizeof(refcount_t);
    if(!ref)
    {
        logError("Failed to get the ref counter of an unassigned reference!");
        return REF_UNASSIGNED; // Maybe return something to mark invalid?
    }
    return *pCount;
}

//make a hashmap for assets
//dont connect to ref