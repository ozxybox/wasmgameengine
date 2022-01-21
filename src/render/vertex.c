#include "vertex.h"
#include <stdlib.h>

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

void vtxbuf_alloc(vtxformat_t format, unsigned int capacity, vtxbuf_t* out)
{
    out->format   = format;
    out->capacity = capacity;
    out->vtxsize  = vtx_size(format);
    out->used     = 0;
    out->vbo      = malloc(out->vtxsize * capacity);
}

void vtxbuf_free(vtxbuf_t* out)
{
    if(out->vbo)
        free(out->vbo);
    
    out->format   = 0;
    out->capacity = 0;
    out->vtxsize  = 0;
    out->used     = 0;
    out->vbo      = 0;
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
void idxbuf_alloc(unsigned int capacity, idxbuf_t* out)
{
    out->capacity = capacity;
    out->used     = 0;
    out->ibo      = (unsigned short*)malloc(sizeof(unsigned short) * capacity);
}
void idxbuf_free(idxbuf_t* out)
{
    if(out->ibo)
        free(out->ibo);
    
    out->capacity = 0;
    out->used     = 0;
    out->ibo      = 0;
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

void idxbuf_reflect(unsigned short* ibo, unsigned int size, idxbuf_t* out)
{
    unsigned int count = size / sizeof(unsigned short);
    out->ibo = ibo;
    out->capacity = count;
    out->used     = count;
}
