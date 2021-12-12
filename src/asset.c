#include "asset.h"
#include "assetdecode.h"
#include "texture.h"
#include "log.h"
#include "primitives.h"
#include "shader.h"

#include <lodepng/lodepng.h>
#include <emscripten/fetch.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

enum ASSET_ARRIVAL_STATE
{
    ASSET_ARRIVAL_FAILED = -1,
    ASSET_ARRIVAL_NONE = 0,
    ASSET_ARRIVAL_DOWNLOADING,
    ASSET_ARRIVAL_DOWNLOADED,
    ASSET_ARRIVAL_DECODING,
    ASSET_ARRIVAL_DECODED,
    ASSET_ARRIVAL_ARRIVED,
};

typedef struct assetInfo_t
{
    const char* name;

    asset_t asset;
    emscripten_fetch_attr_t attr;
    
    int arrived;
    void* arrivedData;

    struct assetInfo_t* next;
} assetInfo_t;

static assetInfo_t* s_pFirstAsset = 0;
static assetInfo_t* s_pLastAsset = 0;

static assetInfo_t* findAsset(int assetClass, const char* name)
{
    if(!name || assetClass == ASSET_CLASS_NONE) return 0;
    for(assetInfo_t* ainf = s_pFirstAsset; ainf; ainf = ainf->next)
    {
        if(ainf->asset.assetClass == assetClass && !strcmp(name, ainf->name))
            return ainf;
    }
    return 0;
}

assetInfo_t* newAssetFetch()
{
    assetInfo_t* ainf = calloc(1, sizeof(assetInfo_t));
    if(s_pLastAsset)
        s_pLastAsset->next = ainf;
    else
        s_pFirstAsset = ainf;
    s_pLastAsset = ainf;

    return ainf;
}

static int s_shouldDecode = 0; 
static int s_needsDecode = 0; 
static pthread_t s_decodingThread;
static void* assetDecodingThread(void* arg);

void preloadSystemInit()
{
    s_shouldDecode = 1;
    pthread_create(&s_decodingThread, 0, assetDecodingThread, 0); 
    logInfo("Launched Asset Decoding Thread");
    
}

void preloadSystemShutdown()
{
    s_shouldDecode = 0;

    assetInfo_t* next = 0;
    for(assetInfo_t* ainf = s_pFirstAsset; ainf; ainf = next)
    {
        //if(ainf->asset.data)
        //    free(ainf->asset.data);
        next = ainf->next;
        free(ainf);
    }

    // This should be done by now. Join it back 
    pthread_join(s_decodingThread, 0);
}

void preloadSystemProcess()
{
    for(assetInfo_t* ainf = s_pFirstAsset; ainf; ainf = ainf->next)
    {
        if(ainf->arrived == ASSET_ARRIVAL_DECODED)
        {
            // We've got the asset decoded, now we just dump it into place

            if(ainf->asset.assetClass == ASSET_CLASS_TEXTURE)
            {
                int w=0, h=0;
                getTextureDimensions(ainf->asset.data, &w, &h);
                loadIntoTexture(ainf->asset.data, ainf->arrivedData, w, h);
                free(ainf->arrivedData);
            }
            else if(ainf->asset.assetClass == ASSET_CLASS_MODEL)
            {
                meshBufferData_t* dat = ainf->arrivedData;
                loadIntoMeshFromArray(ainf->asset.data, dat->vertices, dat->vertexCount, dat->indices, dat->elementCount);
                free(ainf->arrivedData);
            }
            else if(ainf->asset.assetClass == ASSET_CLASS_SHADER)
            {
                shaderParseData_t* dat = ainf->arrivedData;
                loadIntoShader(ainf->asset.data, dat->vs, dat->fs);
                free(dat->buf);
                free(ainf->arrivedData);
            }

            ainf->arrived = ASSET_ARRIVAL_ARRIVED;
            ainf->asset.status = ASSET_STATUS_ARRIVED;
            logInfo("Asset arrived %s", ainf->name);
        }
        else if(ainf->arrived == ASSET_ARRIVAL_FAILED)
        {
            ainf->arrived = 0;
            ainf->asset.status = ASSET_STATUS_FAILED;
        }

    }
}

static void* assetDecodingThread(void* arg)
{
    while(s_shouldDecode)
    {
        while(!s_needsDecode && s_shouldDecode) //sleep(2);
        ;//pthread_sleep(600);
        
        if(!s_shouldDecode) break;

        // Scan for downloaded assets
        for(assetInfo_t* ainf = s_pFirstAsset; ainf; ainf = ainf->next)
        {
            if(ainf->arrived == ASSET_ARRIVAL_DOWNLOADED)
            {
                // Okay, we've got it, now we need to decode it
                ainf->arrived = ASSET_ARRIVAL_DECODING;
                
                emscripten_fetch_t* fetch = ainf->arrivedData;

                if(ainf->asset.assetClass == ASSET_CLASS_TEXTURE)
                {
                    // Assume all images are pngs for now
                    unsigned char* image = 0;
                    unsigned int width, height;

                    unsigned int error = lodepng_decode32(&image, &width, &height, fetch->data, fetch->numBytes);
                    if(error)
                    {
                        logError("Failed to decode png! Error %u: %s\n", error, lodepng_error_text(error));
                        if(image) free(image);
                        ainf->arrived = ASSET_ARRIVAL_FAILED;
                    }
                    else
                    {
                        setTextureDimensions(ainf->asset.data, &width, &height);
                        ainf->arrivedData = image;
                        ainf->arrived = ASSET_ARRIVAL_DECODED;
                    }
                }
                else if(ainf->asset.assetClass == ASSET_CLASS_MODEL)
                {
                    // Assume all meshs are objs for now
                    unsigned char* image = 0;
                    unsigned int width, height;
                    meshBufferData_t* meshdat = malloc(sizeof(meshBufferData_t));
                    *meshdat = loadMeshDataFromObj(fetch->data, fetch->numBytes);

                    ainf->arrivedData = meshdat;
                    ainf->arrived = ASSET_ARRIVAL_DECODED;
                }
                else if(ainf->asset.assetClass == ASSET_CLASS_SHADER)
                {
                    shaderParseData_t* dat = malloc(sizeof(shaderParseData_t));
                    parseShaderFile(fetch->data, dat);
                    /*
                    */
                    logInfo(dat->vs);//str);
                    logInfo(dat->fs);
                    ainf->arrivedData = dat;
                    ainf->arrived = ASSET_ARRIVAL_DECODED;
                }

                // Done with it. Be free!
                emscripten_fetch_close(fetch);
        
                logInfo("Decoded asset %s", ainf->name);
                s_needsDecode--;
            }
        }
        
    }



    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
    return NULL;
}

void assetDownloadSuccess(emscripten_fetch_t* fetch)
{
    assetInfo_t* ainf = fetch->userData;
    ainf->arrivedData = fetch;
    ainf->arrived = ASSET_ARRIVAL_DOWNLOADED;

    // Let 'em know there's new content
    s_needsDecode++;

    logInfo("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
    
}

void assetDownloadError(emscripten_fetch_t *fetch)
{
    assetInfo_t* ainf = fetch->userData;
    ainf->arrived = ASSET_ARRIVAL_FAILED;

    logInfo("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);

    emscripten_fetch_close(fetch);
}

asset_t* preloadAsset(int assetClass, const char* url)
{
    // Do we already have this asset?
    assetInfo_t* ainf = findAsset(assetClass, url);
    if(ainf) return ainf;

    // Nope! Make a new one for it.
    ainf = newAssetFetch();

    // Copy the url in case our caller frees it
    ainf->name = strdup(url);

    ainf->asset.assetClass = assetClass;
    ainf->asset.status = ASSET_STATUS_LOADING;

    // Set data to default for class
    if(assetClass == ASSET_CLASS_TEXTURE)
    {
        ainf->asset.data = emptyTexture();
    }
    else if(assetClass == ASSET_CLASS_MODEL)
    {
        ainf->asset.data = emptyMesh();
        logInfo("New empty %u", ainf->asset.data);
    }
    else if(assetClass == ASSET_CLASS_SHADER)
    {
        ainf->asset.data = emptyShader();
    }
    ainf->arrived = ASSET_ARRIVAL_DOWNLOADING;

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

    logInfo("Begin preload of %s", url);
    return &ainf->asset;
}

texture_t preloadTexture(const char* url)
{
    asset_t* asset = preloadAsset(ASSET_CLASS_TEXTURE, url);
    if(asset)
        return (texture_t)asset->data;
    return 0;
}
asset_t* downloadAsset(int assetClass, const char* url)
{
    return 0;
}


void deleteAsset(asset_t* asset)
{

}
