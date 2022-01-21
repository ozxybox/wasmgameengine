#pragma once
#include "linalg.h"
#include "vertex.h"

#define MAX_MESHES 64
#define MESH_INVALID_INDEX 0xFFFFFFFF 
typedef unsigned int mesh_t;

void mesh_systemInit();
void mesh_systemShutdown();
void mesh_systemReload();

// Empty slot for placing a mesh later
mesh_t mesh_empty();
void mesh_delete(mesh_t mesh);

mesh_t mesh_loadFromArray(vtxbuf_t* vbo, idxbuf_t* ibo);
void mesh_loadIntoFromArray(mesh_t mesh, vtxbuf_t* vbo, idxbuf_t* ibo);
//void mesh_loadIntoDynamicFromArray(mesh_t mesh, void* vertices, int vertexCount, unsigned short* indices, int indexCount);

void mesh_scrub(mesh_t mesh); // Scrubs away data, leaving an empty mesh

void mesh_bind(mesh_t mesh);
void mesh_draw(mesh_t mesh);
void mesh_unbind();

mesh_t mesh_error();
