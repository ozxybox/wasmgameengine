#pragma once
#include "texture.h"
#include "mesh.h"

enum ASSET_CLASS
{
    ASSET_CLASS_NONE,
    ASSET_CLASS_TEXTURE,
    ASSET_CLASS_MODEL,
    //ASSET_CLASS_SOUND,
    ASSET_CLASS_SHADER,
};

enum ASSET_STATUS
{
    ASSET_STATUS_NONE,
    ASSET_STATUS_LOADING,
    ASSET_STATUS_ARRIVED,
    ASSET_STATUS_FAILED,
};

typedef struct asset_t {
    char* name;
    int status;
    int assetclass;
    unsigned int id;
    void* ainf;
    unsigned int pos;
} asset_t;

typedef unsigned int assetIndex_t;

void asset_systemInit();
void asset_systemThinkSlow();
void asset_systemShutdown();

// Begins async download
asset_t* asset_preload(int assetClass, const char* url);

void asset_incref(asset_t* asset);
void asset_decref(asset_t* asset);

assetIndex_t asset_indexof(asset_t* asset);
asset_t* asset_atindex(assetIndex_t index);

// Begins sync download
//asset_t* asset_download(int assetClass, const char* url);

//void asset_delete(asset_t* asset);
