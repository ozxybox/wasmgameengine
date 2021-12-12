#pragma once
#include "linalg.h"

#define MAX_MESHES 64
#define MESH_INVALID_INDEX 0xFFFFFFFF 
typedef unsigned int mesh_t;

typedef struct {
    vec3 pos;
    vec3 norm;
    vec2 uv;
} vertex_t;

void meshSystemInit();
void meshSystemShutdown();

// Empty slot for placing a mesh later
mesh_t emptyMesh();

mesh_t loadMeshFromArray(vertex_t* vertices, int vertexCount, unsigned short* indices, int indexCount);
void loadIntoMeshFromArray(mesh_t mesh, vertex_t* vertices, int vertexCount, unsigned short* indices, int indexCount);
void unloadAllMeshs();

void bindMesh(mesh_t mesh);
void drawMesh(mesh_t mesh);
void unbindMesh();
