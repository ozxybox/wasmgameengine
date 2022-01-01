#pragma once
#include <stddef.h> // for size_t

// 32 refs per allocation block
typedef unsigned int refidx_t;
#define REF_PER_BLOCK 32
#define REF_INVALID 0xFFFFFFFF

// 65k max counts per ref
typedef unsigned short refcount_t;
#define REF_UNASSIGNED 0xFFFF 
#define REF_IMMORTAL 0xFFFE 

typedef void* ref_t;

typedef void(*reffunc_t)(void* mem);

typedef struct refblock_t 
{
    struct refblock_t* next;

    // Allocated here
    //{refcount_t, char[sizeof(type)]} data[REF_PER_BLOCK];
} refblock_t;

typedef struct {
    refblock_t* first;
    refblock_t* current; // The block we're pulling off of

    unsigned int elementsize;
    reffunc_t deconstructor;
} refpool_t;




refpool_t rp_create(unsigned int elementsize, reffunc_t deconstructor);
refpool_t* rp_createNew(unsigned int elementsize, reffunc_t deconstructor);

// Deletes all data within
void rp_clear(refpool_t* refpool);

ref_t rp_push(refpool_t* refpool, void* mem);
void rp_collect(refpool_t* refpool);

void ref_inc(ref_t ref);
void ref_dec(ref_t ref);

void ref_set(ref_t ref, refcount_t count);
refcount_t ref_get(ref_t ref);
