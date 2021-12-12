#include "mesh.h"
#include "primitives.h"
#include <stdlib.h>
#include <stddef.h>
#include <ogl.h>
#include "log.h"

typedef struct meshData_t {
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    
    unsigned int elementCount;
} meshData_t;

static meshData_t* s_pErrorMesh = 0;

static meshData_t s_meshPool[MAX_MESHES];
static unsigned int s_meshCount = 0;

static meshData_t* newMeshData()
{
    meshData_t* dat = s_meshPool + s_meshCount;
    s_meshCount++;
    // This should be clear from meshSystemInit, so we don't have to wipe it
    logWarn("meshs used %u", s_meshCount);
    return dat;
}

static mesh_t meshDataToIdx(meshData_t* data)
{
    size_t off = data - s_meshPool;
    logWarn("off %u", off);

    if(off >= MAX_MESHES)
        return MESH_INVALID_INDEX;
    return off;
}

static void clearMeshData(meshData_t* data)
{
    data->vao = GL_INVALID_INDEX;
    data->vbo = GL_INVALID_INDEX;
    data->ibo = GL_INVALID_INDEX;
    
    data->elementCount = 0;
}

// deletes off the back
static void popMeshs(unsigned int count)
{
    int start = s_meshCount - count;
    if(start < 0 ) start = 0;

    for(int i = start; i < s_meshCount; i++)
    {
        meshData_t* data = &s_meshPool[i];    

        // Delete our gl data
        glDeleteBuffers(1, &data->vbo);
        glDeleteBuffers(1, &data->ibo);
        glDeleteVertexArrays(1, &data->vao);

        // Clear the struct
        clearMeshData(data);
    }

    // Back to the start
    s_meshCount = start;
}

void unloadAllMeshs()
{
    // Keep our first mesh, the error mesh
    popMeshs(s_meshCount-1);
}


static vertex_t s_cubeVbo[];
static unsigned short s_cubeIbo[];

void meshSystemInit()
{
    // Clear our data
    s_meshCount = 0;
    for(int i = 0; i < MAX_MESHES; i++)
        clearMeshData(&s_meshPool[i]);

    // Create the error mesh

}

void meshSystemShutdown()
{
    popMeshs(MAX_MESHES);
}

mesh_t emptyMesh()
{
    meshData_t* dat = newMeshData();
    dat->vao = GL_INVALID_INDEX;
    dat->vbo = GL_INVALID_INDEX;
    dat->ibo = GL_INVALID_INDEX;
    dat->elementCount = 0;
    return meshDataToIdx(dat);
}


void loadIntoMeshFromArray(mesh_t mesh, vertex_t* vertices, int vertexCount, unsigned short* indices, int indexCount)
{   
    if(mesh >= MAX_MESHES) {logInfo("LOL %u", mesh); return;}
    meshData_t* dat = &s_meshPool[mesh];

    GLuint vao, vbo, ibo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * vertexCount, vertices, GL_STATIC_DRAW);
    
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * indexCount, indices, GL_STATIC_DRAW);

    dat->vao = vao;
    dat->vbo = vbo;
    dat->ibo = ibo;
    dat->elementCount = indexCount;
}
mesh_t loadMeshFromArray(vertex_t* vertices, int vertexCount, unsigned short* indices, int indexCount)
{
    
    meshData_t* dat = newMeshData();
    mesh_t mesh = meshDataToIdx(dat);
    loadIntoMeshFromArray(mesh, vertices, vertexCount, indices, indexCount);

    return mesh;
}

void bindMesh(mesh_t mesh)
{
    if(mesh >= MAX_MESHES) {mesh = cubeMesh();}
    meshData_t* dat = &s_meshPool[mesh];
    if(dat->vao == GL_INVALID_INDEX || dat->vbo == GL_INVALID_INDEX || dat->ibo == GL_INVALID_INDEX) {bindMesh(cubeMesh()); return;}


    glBindVertexArray(dat->vao);
	glBindBuffer(GL_ARRAY_BUFFER, dat->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dat->ibo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)offsetof(vertex_t, pos));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)offsetof(vertex_t, norm));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)offsetof(vertex_t, uv));
}


void drawMesh(mesh_t mesh)
{
    if(mesh >= MAX_MESHES) {mesh = cubeMesh();}
    meshData_t* dat = &s_meshPool[mesh];
    if(dat->vao == GL_INVALID_INDEX || dat->vbo == GL_INVALID_INDEX || dat->ibo == GL_INVALID_INDEX) {bindMesh(cubeMesh()); return;}

	glDrawElements(GL_TRIANGLES, dat->elementCount, GL_UNSIGNED_SHORT, 0);
}
void unbindMesh()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
