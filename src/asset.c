#include "asset.h"
#include "assetdecode.h"
#include "texture.h"
#include "log.h"
#include "primitives.h"
#include "shader.h"
#include "hashmap.h"
#include "refpool.h"
#include "linkedlist.h"

#include <lodepng/lodepng.h>
#include <emscripten/fetch.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


static const char* s_assetClassNames[] = {
    "none",
    "texture",
    "model",
    //"sound",
    "shader",
};

enum ASSET_ARRIVAL_STATE
{
    ASSET_ARRIVAL_FAILED = -1,
    ASSET_ARRIVAL_NONE = 0,
    ASSET_ARRIVAL_DOWNLOADING,
    ASSET_ARRIVAL_DOWNLOADED,
    ASSET_ARRIVAL_DECODING,
    ASSET_ARRIVAL_DECODED,
    ASSET_ARRIVAL_ARRIVED,
    ASSET_ARRIVAL_CANCELED,
};

typedef struct assetLoadInfo_t
{
    const char* name;
    unsigned int assetclass;
    unsigned int id;
    
    emscripten_fetch_attr_t attr;
    int arrived;
    void* arrivedData;

    asset_t* asset;
} assetLoadInfo_t;


// Connects asset names to ref_t's of asset_t 
static hashmap_t s_assetMap;
// Stores all asset data and refs
static refpool_t s_assetPool;
// Assets currently being downloaded by emscripten
static linklist_t s_assetInFlightList;
// Assets currently being worked on by the decoding thread
static linklist_t s_assetDecodingList;
// Assets currently being worked on by the main thread
static linklist_t s_assetArrivalsList;

// Threading info
static int s_shouldDecode = 0; 
static int s_decoding = 0; 
static pthread_t s_decodingThread;

///////////////////////////////////
// IMPLEMENT ASSET CLASSES BELOW //
///////////////////////////////////

static void blankAsset(asset_t* asset)
{
    switch(asset->assetclass)
    {
        case ASSET_CLASS_TEXTURE:
            asset->id = texture_empty();
            break;
        case ASSET_CLASS_MODEL:    
            asset->id = mesh_empty();
            break;
        case ASSET_CLASS_SHADER: 
            asset->id = shader_empty();
            break;
        default:
            break;
    }
}

static void freeAsset(asset_t* asset)
{
    switch(asset->assetclass)
    {
        case ASSET_CLASS_TEXTURE:
            texture_delete(asset->id);
            break;
        case ASSET_CLASS_MODEL:
            mesh_delete(asset->id);
            break;
        case ASSET_CLASS_SHADER: 
            shader_delete(asset->id);
            break;
        default:
            break;
    }
}

static void buildAsset(assetLoadInfo_t* ainf)
{
    if(ainf->assetclass == ASSET_CLASS_TEXTURE)
    {
        unsigned int w=0, h=0;
        texture_getDimensions(ainf->id, &w, &h);
        texture_loadInto(ainf->id, ainf->arrivedData, w, h);
    }
    else if(ainf->assetclass == ASSET_CLASS_MODEL)
    {
        meshBufferData_t* dat = ainf->arrivedData;
        mesh_loadIntoFromArray(ainf->id, dat->vertices, dat->vertexCount, dat->indices, dat->elementCount);
    }
    else if(ainf->assetclass == ASSET_CLASS_SHADER)
    {
        shaderParseData_t* dat = ainf->arrivedData;
        shader_loadInto(ainf->id, dat->vs, dat->fs);
        free(dat->buf);
    }
}

void decodeAsset(assetLoadInfo_t* ainf, emscripten_fetch_t* fetch)
{
    if(ainf->assetclass == ASSET_CLASS_TEXTURE)
    {
        // Assume all images are pngs for now
        unsigned char* image = 0;
        unsigned int width, height;

        unsigned int error = lodepng_decode32(&image, &width, &height, fetch->data, fetch->numBytes);
        if(error)
        {
            logError("[asset] Failed to decode png! Error %u: %s\n", error, lodepng_error_text(error));
            if(image) free(image);
            ainf->arrived = ASSET_ARRIVAL_FAILED;
        }
        else
        {
            texture_setDimensions(ainf->id, &width, &height);
            ainf->arrivedData = image;
            ainf->arrived = ASSET_ARRIVAL_DECODED;
        }
    }
    else if(ainf->assetclass == ASSET_CLASS_MODEL)
    {
        // Assume all meshs are objs for now
        unsigned char* image = 0;
        unsigned int width, height;
        meshBufferData_t* meshdat = malloc(sizeof(meshBufferData_t));
        *meshdat = loadMeshDataFromObj(fetch->data, fetch->numBytes);

        ainf->arrivedData = meshdat;
        ainf->arrived = ASSET_ARRIVAL_DECODED;
    }
    else if(ainf->assetclass == ASSET_CLASS_SHADER)
    {
        shaderParseData_t* dat = malloc(sizeof(shaderParseData_t));
        parseShaderFile(fetch->data, dat);

        ainf->arrivedData = dat;
        ainf->arrived = ASSET_ARRIVAL_DECODED;
    }

}
///////////////////////////////////
// IMPLEMENT ASSET CLASSES ABOVE //
///////////////////////////////////


void assetRefDeconstructor(void* mem)
{
    asset_t* asset = (asset_t*)mem;
    if(asset->status == ASSET_STATUS_ARRIVED)
        logInfo("[asset] Deleting %s", asset->name);
    else if(asset->status == ASSET_STATUS_FAILED)
        logInfo("[asset] Clearing %s", asset->name);
    else if(asset->status == ASSET_STATUS_LOADING)
    {
        logInfo("[asset] Canceling %s", asset->name);

        // *Only* time we should access this
        assetLoadInfo_t* ainf = (assetLoadInfo_t*)asset->ainf;
        ainf->asset = 0;
    }
    
    freeAsset(asset);
    hm_remove(&s_assetMap, asset->name);
    free(asset->name);
}

/*
static assetLoadInfo_t* findAsset(const char* name)
{
    assetLoadInfo_t* asset = 0;
    hm_get(&s_assetMap, name, &asset);
    return asset;
}
*/

static void* assetDecodingThread(void* arg);

void asset_systemInit()
{
    logInfo("[asset] System Init");

    s_assetMap = hm_create();
    s_assetPool = rp_create(sizeof(asset_t), assetRefDeconstructor);

    s_assetInFlightList = ll_create(sizeof(assetLoadInfo_t));
    s_assetDecodingList = ll_create(sizeof(assetLoadInfo_t));
    s_assetArrivalsList = ll_create(sizeof(assetLoadInfo_t));


    // Should be last!
    s_decoding = 0;
    s_shouldDecode = 1;
    pthread_create(&s_decodingThread, 0, assetDecodingThread, 0); 
    logInfo("[asset] Launched Decoding Thread");
}

void asset_systemShutdown()
{
    logInfo("[asset] System Shutdown");

    s_shouldDecode = 0;
 
    // This should be done by now. Join it back
    pthread_join(s_decodingThread, 0);

    // Clear the map first
    hm_clear(&s_assetMap);
    rp_clear(&s_assetPool);
    ll_clear(&s_assetInFlightList);
}

void asset_systemThinkSlow()
{
    // Should we ship off more data to the decoder?
    if(!s_decoding && s_assetDecodingList.count == 0)
    {
        // While the decoder is stalled, let's empty the trash

        int cleanCount = 0;

        // Clean up old downloads
        linkitem_t* n = 0;
        for(linkitem_t* li = s_assetArrivalsList.first; li; li = n)
        {
            n = li->next;

            assetLoadInfo_t* ainf = ll_access(li);
            if(ainf->arrived == ASSET_ARRIVAL_ARRIVED)
            {
                // Delete the info
                free(ainf->name);
                ll_remove(&s_assetArrivalsList, li);
                cleanCount++;
            }            
        }

        // Clean up failed or canceled downloads
        n = 0;
        for(linkitem_t* li = s_assetInFlightList.first; li; li = n)
        {
            n = li->next;

            assetLoadInfo_t* ainf = ll_access(li);
            // FIXME: Due to a emscripten bug, we can't cancel this until we're done downloading
            if(ainf->arrived != ASSET_ARRIVAL_DOWNLOADING && (ainf->arrived == ASSET_ARRIVAL_FAILED || ainf->arrived == ASSET_ARRIVAL_CANCELED || !ainf->asset))
            {
                // Tag the asset for pruning
                if(ainf->asset)
                {
                    ainf->asset->status = ASSET_STATUS_FAILED;
                    ref_set(ainf->asset, 0);
                }

                // Delete the info
                free(ainf->name);
                ll_remove(&s_assetInFlightList, li);
                cleanCount++;
            }
        }
        if(cleanCount) logInfo("[asset] Cleaned %d infos", cleanCount);

        // Anything for the decoder?
        if(s_assetInFlightList.count > 0)
        {
            logInfo("[asset] Pushing off %u items for decoding!", s_assetInFlightList.count);
            
            // Copy off the *entire* list to the decoder
            ll_attachlist(&s_assetDecodingList, &s_assetInFlightList);
        }

    }

    // Check if we got anything back from the decoding thread
    for(linkitem_t* li = s_assetArrivalsList.first; li; li = li->next)
    {
        assetLoadInfo_t* ainf = ll_access(li);
        
        if(ainf->arrived == ASSET_ARRIVAL_CANCELED || !ainf->asset)
        {
            // Asset got canceled?
            ainf->arrived = ASSET_ARRIVAL_CANCELED;
            ainf->arrived = ASSET_ARRIVAL_ARRIVED;
        }
        if(ainf->arrived == ASSET_ARRIVAL_DECODED)
        {
            // We've got the asset decoded, now we just dump it into place
            buildAsset(ainf);

            if(ainf->arrivedData) free(ainf->arrivedData);
            ainf->arrived = ASSET_ARRIVAL_ARRIVED;
            ainf->asset->status = ASSET_STATUS_ARRIVED;
            logInfo("[asset] Arrived %s", ainf->name);
        }
        else if(ainf->arrived == ASSET_ARRIVAL_FAILED)
        {
            ainf->arrived = ASSET_ARRIVAL_ARRIVED;
            ainf->asset->status = ASSET_STATUS_FAILED;
            logError("[asset] Failed %s", ainf->name);
        }
    }

    // Collect dead assets
    rp_collect(&s_assetPool);
}

static void* assetDecodingThread(void* arg)
{
    logInfo("[asset] Hello from the other siiide");
    while(s_shouldDecode)
    {
        while(!s_assetDecodingList.count && s_shouldDecode);
        if(!s_shouldDecode) break;
        //{
            //sleep(2);
            //pthread_sleep(600);
        //}
            //logInfo("Stall");
        

        //logInfo("Begin decode");

        // Scan for downloaded assets
        s_decoding = 1;
        linkitem_t* n = 0;
        for(linkitem_t* li = s_assetDecodingList.first; li; li = n)
        {
            n = li->next; // Mark the next item first, in case we remove it 
            assetLoadInfo_t* ainf = ll_access(li);

            //logInfo("Checking %s", ainf->name);

            // Has this been canceled?
            if(ainf->arrived == ASSET_ARRIVAL_CANCELED || !ainf->asset)
            {
                // Done with it. Be free!
                emscripten_fetch_close(ainf->arrivedData);
        
                logInfo("[asset] Canceled Decode %s", ainf->name);

                free(ainf->name);
                ll_remove(&s_assetDecodingList, li);
            }
            else if(ainf->arrived == ASSET_ARRIVAL_DOWNLOADED)
            {
                // Okay, we've got it, now we need to decode it
                ainf->arrived = ASSET_ARRIVAL_DECODING;
                
                emscripten_fetch_t* fetch = ainf->arrivedData;

                decodeAsset(ainf, fetch);

                // Done with it. Be free!
                emscripten_fetch_close(fetch);
                
        
                logInfo("[asset] Decoded %s", ainf->name);

                // Move this to the done pile
                ll_popfrompushto(&s_assetArrivalsList, &s_assetDecodingList, li);
            }
        }
        s_decoding = 0;
    }

    logInfo("[asset] Goodbye from the other siiide");

    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
    return NULL;
}

void assetDownloadSuccess(emscripten_fetch_t* fetch)
{
    assetLoadInfo_t* ainf = fetch->userData;
    ainf->arrivedData = fetch;

    // Was this canceled?
    if(!ainf->asset)
    {
        // Just close the fetch now, since we were canceled
        ainf->arrived = ASSET_ARRIVAL_CANCELED;
        emscripten_fetch_close(fetch);
    }
    else
        ainf->arrived = ASSET_ARRIVAL_DOWNLOADED;

    logInfo("[asset] Downloaded %llu bytes from %s", fetch->numBytes, fetch->url);
}

void assetDownloadError(emscripten_fetch_t *fetch)
{
    assetLoadInfo_t* ainf = fetch->userData;
    ainf->arrived = ASSET_ARRIVAL_FAILED;

    logInfo("[asset] Downloading %s failed, HTTP failure status code: %d", fetch->url, fetch->status);

    emscripten_fetch_close(fetch);
}

asset_t* asset_preload(int assetclass, const char* url)
{
    if(!url) return 0;

    // Do we already have this asset?
    asset_t* asset = 0;
    if(hm_get(&s_assetMap, url, &asset)) 
        return asset;

    // Nope! Make a new one for it.
    char* name = strdup(url); // Copy the url for safety
    asset_t u = {
        .name = name,
        .assetclass = assetclass,
        .status = ASSET_STATUS_LOADING,
    };

    // Put the data in a ref and in the map
    ref_t ref = rp_push(&s_assetPool, &u);
    hm_set(&s_assetMap, name, ref);
    asset = ref;
    hm_indexof(&s_assetMap, name, &asset->pos);

    // Set up the asset for its class
    blankAsset(asset);


    // Set up our load info for while we're downloading
    assetLoadInfo_t v = {
        .name = strdup(url), // Yes, two duplicates
        .assetclass = assetclass,
        .id = asset->id,

        .arrived = ASSET_ARRIVAL_DOWNLOADING,
        .arrivedData = 0,
        
        .asset = asset,
    };
    linkitem_t* li = ll_push(&s_assetInFlightList, &v);
    assetLoadInfo_t* ainf = ll_access(li);

    asset->ainf = ainf;

    // Prep emscripten for fetching data
    emscripten_fetch_attr_t* attr = &ainf->attr;
    emscripten_fetch_attr_init(attr);
    strcpy(attr->requestMethod, "GET");
    attr->attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr->onsuccess = assetDownloadSuccess;
    attr->onerror = assetDownloadError;

    // Pass a pointer to the fetch data for the user data for later access
    attr->userData = ainf;

    // Fetch the data
    emscripten_fetch(attr, url);

    logInfo("[asset] Preload %s: %s -> %u", s_assetClassNames[assetclass], url, asset->id);
    return asset;
}

assetIndex_t asset_indexof(asset_t* asset)
{
    return asset->pos;
}
asset_t* asset_atindex(assetIndex_t index)
{
    hashmappair_t* pair;
    if(hm_at(&s_assetMap, index, &pair))
        return pair->value;
    return 0;
}


void asset_incref(asset_t* asset)
{
    ref_inc(asset);
}

void asset_decref(asset_t* asset)
{
    ref_dec(asset);
}