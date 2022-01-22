#include "vertex.h"
#include <stdlib.h>
#include <string.h>

///////////////////
// Vertex Format //
///////////////////

unsigned int vtx_size(vtxformat_t fmt)
{
    unsigned int sz = 0;

    // Enabled always
    //if(fmt & VTX_POSITION)
        sz += sizeof(vec3);

    if(fmt & VTX_NORMAL)
        sz += sizeof(vec3);

    if(fmt & VTX_TEXCOORD)
        sz += sizeof(vec2);
        
    if(fmt & VTX_COLOR)
        sz += sizeof(vec2);
        
    return sz;
}

// Right now, if an attribute is disabled, it will still point somewhere
// Might be worth it to try to resolve that if it becomes an issue

unsigned int vtx_offsetpos(vtxformat_t fmt)
{
    // Position is always enabled and first
    return 0;
}
unsigned int vtx_offsetnorm(vtxformat_t fmt)
{
    // Position is enabled always
    unsigned int o = sizeof(vec3);

    return o;
}
unsigned int vtx_offsetuv(vtxformat_t fmt)
{
    // Position is enabled always
    unsigned int o = sizeof(vec3);

    if(fmt & VTX_NORMAL)
        o += sizeof(vec3);

    return o;
}
unsigned int vtx_offsetcolor(vtxformat_t fmt)
{
    // Position is enabled always
    unsigned int o = sizeof(vec3);

    if(fmt & VTX_NORMAL)
        o += sizeof(vec3);
    
    if(fmt & VTX_TEXCOORD)
        o += sizeof(vec2);
    
    return o;
}

vec3* vtx_pos(vtxformat_t fmt, vertex_t* vert)
{
    return (vec3*)vert;
}
vec3* vtx_norm(vtxformat_t fmt, vertex_t* vert)
{
    return (vec3*)(vtx_offsetnorm(fmt) + (char*)vert);
}
vec2* vtx_uv(vtxformat_t fmt, vertex_t* vert)
{
    return (vec3*)(vtx_offsetuv(fmt) + (char*)vert);
}
vec3* vtx_color(vtxformat_t fmt, vertex_t* vert)
{
    return (vec3*)(vtx_offsetcolor(fmt) + (char*)vert);
}

///////////////////
// Vertex Buffer //
///////////////////

void vtxbuf_alloc(vtxbuf_t* vb, vtxformat_t format, unsigned int capacity)
{
    vb->format   = format;
    vb->capacity = capacity;
    vb->vtxsize  = vtx_size(format);
    vb->used     = 0;
    vb->vbo      = malloc(vb->vtxsize * capacity);
}

void vtxbuf_free(vtxbuf_t* vb)
{
    if(vb->vbo)
        free(vb->vbo);
    
    vb->format   = 0;
    vb->capacity = 0;
    vb->vtxsize  = 0;
    vb->used     = 0;
    vb->vbo      = 0;
}

void vtxbuf_fit(vtxbuf_t* vb, unsigned int count)
{
    if(vb->capacity >= count)
        return;
    
    vertex_t* n = malloc(count * vb->vtxsize);
    memcpy(n, vb->vbo, vb->capacity * vb->vtxsize);
    free(vb->vbo);

    vb->vbo = n;
    vb->capacity = count;
}


vertex_t* vtxbuf_eat(vtxbuf_t* vb)
{
    return vtxbuf_gorge(vb, 1);
}
vertex_t* vtxbuf_gorge(vtxbuf_t* vb, unsigned int count)
{
    if(vb->used + count > vb->capacity) 
        return 0;

    vertex_t* v = (vertex_t*)((char*)vb->vbo + vb->vtxsize * vb->used);
    vb->used += count;
    return v;
}

void vtxbuf_reset(vtxbuf_t* vb)
{
    vb->used = 0;
}

void vtxbuf_reflect(vtxformat_t format, vertex_t* vbo, unsigned int size, vtxbuf_t* out)
{
    unsigned int sz = vtx_size(format);
    unsigned int count = size / sz;
    out->vbo = vbo;
    out->format   = format;
    out->capacity = count;
    out->used     = count;
    out->vtxsize  = sz;
}



// Index buffer
void idxbuf_alloc(idxbuf_t* ib, unsigned int capacity)
{
    ib->capacity = capacity;
    ib->used     = 0;
    ib->ibo      = (unsigned short*)malloc(sizeof(unsigned short) * capacity);
}
void idxbuf_free(idxbuf_t* ib)
{
    if(ib->ibo)
        free(ib->ibo);
    
    ib->capacity = 0;
    ib->used     = 0;
    ib->ibo      = 0;
}

void idxbuf_fit(idxbuf_t* ib, unsigned int count)
{
    if(ib->capacity >= count)
        return;
    
    vertex_t* n = malloc(count * sizeof(unsigned short));
    memcpy(n, ib->ibo, ib->capacity * sizeof(unsigned short));
    free(ib->ibo);
    
    ib->ibo = n;
    ib->capacity = count;
}

unsigned short* idxbuf_eat(idxbuf_t* ib)
{
   return idxbuf_gorge(ib, 1);
}
unsigned short* idxbuf_gorge(idxbuf_t* ib, unsigned int count)
{
    if(ib->used + count > ib->capacity) 
        return 0;

    unsigned short* i = ib->ibo + ib->used;
    ib->used+=count;
    return i;
}

void idxbuf_reset(idxbuf_t* ib)
{
    ib->used = 0;
}

void idxbuf_reflect(unsigned short* ibo, unsigned int size, idxbuf_t* out)
{
    unsigned int count = size / sizeof(unsigned short);
    out->ibo = ibo;
    out->capacity = count;
    out->used     = count;
}
