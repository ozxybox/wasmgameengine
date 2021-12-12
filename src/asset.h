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
    int status;
    int assetClass;
    unsigned int data; // Will be in the format of the class type
} asset_t;

// Begins async download
asset_t* preloadAsset(int assetClass, const char* url);

texture_t preloadTexture(const char* url);

// Begins sync download
asset_t* downloadAsset(int assetClass, const char* url);

void deleteAsset(asset_t* asset);

void preloadSystemInit();
void preloadSystemProcess();
void preloadSystemShutdown();