#include "mesh.h"
#include "primitives.h"
#include "log.h"
#include "linkedlist.h"
#include "map.h"
#include <ogl.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct meshData_t {
    mesh_t id;

    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    
    unsigned int elementCount;
    vtxformat_t format;
    
} meshData_t;

static meshData_t* s_pErrorMesh = 0;

static linklist_t s_meshList;
static map_t s_meshMap;
static unsigned int s_meshId;

//////////////////////
// Static functions //
//////////////////////

static meshData_t* newMeshData()
{
    // Get a new key id
    unsigned int k = s_meshId;
    s_meshId++;

    // Set up the data
    meshData_t m = {
        .id = k,

        .vao = GL_INVALID_INDEX,
        .vbo = GL_INVALID_INDEX,
        .ibo = GL_INVALID_INDEX,

        .elementCount = 0
    };

    // Make space for it on the list
    linkitem_t* li = ll_push(&s_meshList, &m);
    void* dat = ll_access(li);

    // Place it in the map
    map_set(&s_meshMap, k, dat);

    // Pass back the mesh
    return (meshData_t*)dat;
}

static meshData_t* meshDataFromId(mesh_t mesh)
{
    // Pull in our data from our id
    meshData_t* dat;
    int success = map_get(&s_meshMap, mesh, (void**)&dat);
    if(!success)
    {
        // FIXME: Too annyoing // logError("[mesh] (%u) does not exist!", mesh);
        dat = s_pErrorMesh;
    }
    else if(dat->vao == GL_INVALID_INDEX || dat->vbo == GL_INVALID_INDEX || dat->ibo == GL_INVALID_INDEX)
    {
        //logError("[mesh] (%u) invalid!", mesh);
        // Probably waiting for this model to load. No shouting
        dat = s_pErrorMesh;
    }

    return dat;
}


static void deleteMeshGL(meshData_t* data)
{
    // Delete our gl data
    if(data->vbo != GL_INVALID_INDEX)
        glDeleteBuffers(1, &data->vbo);
    if(data->ibo != GL_INVALID_INDEX)
        glDeleteBuffers(1, &data->ibo);
    if(data->vao != GL_INVALID_INDEX)
        glDeleteVertexArrays(1, &data->vao);
}


static void loadMeshData(meshData_t* dat, vtxbuf_t* vtxs, idxbuf_t* idxs)
{
    GLuint vao, vbo, ibo;
    
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vtxs->used * vtxs->vtxsize, vtxs->vbo, GL_STATIC_DRAW);
    
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * idxs->used, idxs->ibo, GL_STATIC_DRAW);

    dat->vao = vao;
    dat->vbo = vbo;
    dat->ibo = ibo;
    dat->elementCount = idxs->used;
    dat->format = vtxs->format;
}

extern vtxprim_t g_meshErrorVBO[];
extern unsigned short g_meshErrorIBO[];
extern int g_meshErrorVN;
extern int g_meshErrorIN;

static void createErrorMesh()
{
    // Create the error mesh
    s_pErrorMesh = newMeshData();

	vtxbuf_t vbo;
    idxbuf_t ibo;
	vtxbuf_reflect(VTXPRIM_FORMAT, g_meshErrorVBO, g_meshErrorVN, &vbo);
	idxbuf_reflect(g_meshErrorIBO, g_meshErrorIN, &ibo);
    loadMeshData(s_pErrorMesh, &vbo, &ibo);
}


///////////////////////
// Exposed Functions //
///////////////////////

void mesh_systemInit()
{
    logInfo("[mesh] System Init");

    s_meshId = 0;
    s_meshList = ll_create(sizeof(meshData_t));
    s_meshMap = map_create();

    // Create error mesh
    createErrorMesh();
}

void mesh_systemShutdown()
{
    logInfo("[mesh] System Shutdown");

    // Clear out all GL data
    for(linkitem_t* li = s_meshList.first; li; li = li->next)
        deleteMeshGL(ll_access(li));

    // Clear out the lists
    ll_clear(&s_meshList);
    map_clear(&s_meshMap);
}

void mesh_systemReload()
{
    mesh_systemShutdown();
    mesh_systemInit();
}

mesh_t mesh_empty()
{
    return newMeshData()->id;
}


void mesh_delete(mesh_t mesh)
{
    meshData_t* data;
    if(map_get(&s_meshMap, mesh, (void**)&data))  
    {
        deleteMeshGL(data);

        // Remove from the map and list
        map_remove(&s_meshMap, data->id);
        ll_remove(&s_meshList, ll_item(data));
    }
}


void mesh_loadIntoFromArray(mesh_t mesh, vtxbuf_t* vbo, idxbuf_t* ibo)
{   
    meshData_t* data;
    if(map_get(&s_meshMap, mesh, (void**)&data))
        loadMeshData(data, vbo, ibo);
    //else
        // FIXME: Too annyoing //logError("[mesh] (%u) does not exist!", mesh);
}

mesh_t mesh_loadFromArray(vtxbuf_t* vbo, idxbuf_t* ibo)
{
    meshData_t* data = newMeshData();
    loadMeshData(data, vbo, ibo);

    return data->id;
}

void mesh_scrub(mesh_t mesh)
{
    meshData_t* data;
    if(map_get(&s_meshMap, mesh, (void**)&data))
    {
        deleteMeshGL(data);
        data->vbo = GL_INVALID_INDEX;
        data->ibo = GL_INVALID_INDEX;
        data->vao = GL_INVALID_INDEX;
        data->elementCount = 0;
    }
}

void mesh_bind(mesh_t mesh)
{
    meshData_t* data = meshDataFromId(mesh);
    
    // Bind up the mesh
    glBindVertexArray(data->vao);
	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->ibo);

    vtxformat_t fmt = data->format;
    unsigned int sz = vtx_size(fmt);

    // Position is always enabled
    {
    	glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sz, (const void*)vtx_offsetpos(fmt));
    }
    if(fmt & VTX_NORMAL)
    {
    	glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sz, (const void*)vtx_offsetnorm(fmt));
    }
	if(fmt & VTX_TEXCOORD)
    {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sz, (const void*)vtx_offsetuv(fmt));
    }
	if(fmt & VTX_COLOR)
    {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sz, (const void*)vtx_offsetcolor(fmt));
    }
}


void mesh_draw(mesh_t mesh)
{
    meshData_t* data = meshDataFromId(mesh);

	glDrawElements(GL_TRIANGLES, data->elementCount, GL_UNSIGNED_SHORT, 0);
}

void mesh_unbind()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

mesh_t mesh_error()
{
    return s_pErrorMesh->id;
}
