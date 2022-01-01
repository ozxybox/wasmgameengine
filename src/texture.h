#pragma once

typedef unsigned int texture_t;

void texture_systemInit();
void texture_systemShutdown();
void texture_systemReload();

// Empty slot for placing a texture later
texture_t texture_empty();
void texture_delete(texture_t texture);

// Creates a texture from data
texture_t texture_load(unsigned char* pixels, unsigned int width, unsigned int height);
// Loads data into an empty texture
void texture_loadInto(texture_t texture, unsigned char* pixels, unsigned int width, unsigned int height);

void texture_setDimensions(texture_t texture, unsigned int* w, unsigned int* h); // Null to not set
void texture_getDimensions(texture_t texture, unsigned int* w, unsigned int* h); // Null to not get

void texture_bind(texture_t texture);
texture_t texture_error();