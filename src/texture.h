#pragma once

#define MAX_TEXTURES 64
#define TEXTURE_INVALID_INDEX 0xFFFFFFFF 
typedef unsigned int texture_t;


void textureSystemInit();
void textureSystemShutdown();

texture_t errorTexture();

// Creates a texture from data
texture_t loadTexture(unsigned char* pixels, unsigned int width, unsigned int height);

// Empty slot for placing a texture later
texture_t emptyTexture();

// Loads data into an empty texture
void loadIntoTexture(texture_t texture, unsigned char* pixels, unsigned int width, unsigned int height);

void setTextureDimensions(texture_t texture, int* w, int* h); // Null to not set
void getTextureDimensions(texture_t texture, int* w, int* h); // Null to not get

void bindTexture(texture_t tex);