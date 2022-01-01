#pragma once
#include "texture.h"
#include "mesh.h"


texture_t texture_loadFromPNG(const unsigned char* png, unsigned int size);
void texture_loadIntoFromPNG(texture_t texture, const unsigned char* png, unsigned int size);
//mesh_t loadModelFromObj(const unsigned char* obj, unsigned int size);
//void loadIntoModelFromObj(mesh_t mesh, const unsigned char* obj, unsigned int size);

typedef struct 
{
    vertex_t* vertices;
    unsigned int vertexCount;

    unsigned short* indices;
    unsigned int elementCount;
} meshBufferData_t;

meshBufferData_t loadMeshDataFromObj(const unsigned char* obj, unsigned int size);

typedef struct 
{
    // fs and vs just point to buf. Free buf when done.
    char* buf;

    char* vs;
    int vsSize;
    
    char* fs;
    int fsSize;
} shaderParseData_t;

void parseShaderFile(char* shader, shaderParseData_t* out );