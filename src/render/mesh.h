#pragma once
#include "linalg.h"

#define MAX_MESHES 64
#define MESH_INVALID_INDEX 0xFFFFFFFF 
typedef unsigned int mesh_t;

typedef struct vertex_t {
    vec3 pos;
    vec3 norm;
    vec2 uv;
} vertex_t;

void mesh_systemInit();
void mesh_systemShutdown();
void mesh_systemReload();

// Empty slot for placing a mesh later
mesh_t mesh_empty();
void mesh_delete(mesh_t mesh);

mesh_t mesh_loadFromArray(vertex_t* vertices, int vertexCount, unsigned short* indices, int indexCount);
void mesh_loadIntoFromArray(mesh_t mesh, vertex_t* vertices, int vertexCount, unsigned short* indices, int indexCount);

void mesh_bind(mesh_t mesh);
void mesh_draw(mesh_t mesh);
void mesh_unbind();

mesh_t mesh_error();
