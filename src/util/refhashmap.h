#include "refpool.h"
#include "hashmap.h"


// Reference counting hashmap

typedef struct refhashmap_t
{
    hashmap_t hashmap;
    refpool_t refpool;
} refhashmap_t;


refhashmap_t rhm_create(unsigned int elementsize, reffunc_t deconstructor);
void rhm_clear(refhashmap_t* map);

// Copies value. Do NOT delete KEY.
void rhm_set(refhashmap_t* map, const char* key, void* value);
// 1 on success
//int rhm_remove(refhashmap_t* map, const char* key);
int rhm_get(refhashmap_t* map, const char* key, ref_t* out);
int rhm_inc(refhashmap_t* map, const char* key);
int rhm_dec(refhashmap_t* map, const char* key);
