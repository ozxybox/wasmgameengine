#pragma once
#include "linalg.h"

///////////////////
// Vertex Format //
///////////////////

// Vertex format bitflags
typedef int vtxformat_t;
enum 
{
    VTX_POSITION = 0, // We've got at least a position for everything
    VTX_NORMAL   = 1,
    VTX_TEXCOORD = 2,
    VTX_COLOR    = 4,

    VTX_ALL      = 7
};

typedef void vertex_t;

// All attributes
// VTX_POSITION | VTX_TEXCOORD | VTX_NORMAL | VTX_COLOR
typedef struct vertexfull_t {
    vec3 pos;
    vec3 norm;
    vec2 uv;
    vec3 color;
} vertexfull_t;


// Vertex accessors
unsigned int vtx_size(vtxformat_t format);

unsigned int vtx_offsetpos  (vtxformat_t fmt);
unsigned int vtx_offsetnorm (vtxformat_t fmt);
unsigned int vtx_offsetuv   (vtxformat_t fmt);
unsigned int vtx_offsetcolor(vtxformat_t fmt);

vec3* vtx_pos  (vtxformat_t fmt, vertex_t* vert);
vec3* vtx_norm (vtxformat_t fmt, vertex_t* vert);
vec2* vtx_uv   (vtxformat_t fmt, vertex_t* vert);
vec3* vtx_color(vtxformat_t fmt, vertex_t* vert);

///////////////////
// Vertex Buffer //
///////////////////

typedef struct vtxbuf_t
{
    vertex_t* vbo;
    vtxformat_t format;
    unsigned int capacity;
    unsigned int vtxsize;
    unsigned int used;
} vtxbuf_t;

void vtxbuf_alloc(vtxbuf_t* vb, vtxformat_t format, unsigned int capacity);
void vtxbuf_free(vtxbuf_t* vb);

// Grow to fit at least this count
void vtxbuf_fit(vtxbuf_t* vb, unsigned int count);

// This acts more as a consuming function than anything else.
// The first time it gets called, it returns the first vertex
// Second time, the second vert, third the third, etc.
// Each time it marks that another vert has been used up
//
// Maybe it should have a void return and copy data in 
// from an argument, but this seems to work just perfectly.
vertex_t* vtxbuf_eat(vtxbuf_t* vb);
vertex_t* vtxbuf_gorge(vtxbuf_t* vb, unsigned int count); // eat many

// Puts our current vertex back to the start
// Does not free any memory
void vtxbuf_reset(vtxbuf_t* vb);

// Just shows the state of an existing array of vertices. Not to be freed! 
void vtxbuf_reflect(vtxformat_t format, vertex_t* vbo, unsigned int size, vtxbuf_t* out);



// Index buffer
// Functionally the exact same as the vertex buffer, but for indices

typedef struct idxbuf_t
{
    unsigned short* ibo;
    unsigned int capacity;
    unsigned int used;
} idxbuf_t;

void idxbuf_alloc(idxbuf_t* ib, unsigned int capacity);
void idxbuf_free(idxbuf_t* ib);

void idxbuf_fit(idxbuf_t* ib, unsigned int count);

unsigned short* idxbuf_eat(idxbuf_t* ib);
unsigned short* idxbuf_gorge(idxbuf_t* ib, unsigned int count); // eat many

void idxbuf_reset(idxbuf_t* ib);

// Just shows the state of an existing array of indices. Not to be freed! 
void idxbuf_reflect(unsigned short* ibo, unsigned int size, idxbuf_t* out);


//////////////////////////////////
// Commonly Used Vertex Formats //
//////////////////////////////////

// This is often used by the built in geometry in the engine
#define VTXPRIM_FORMAT (VTX_POSITION | VTX_NORMAL | VTX_TEXCOORD)
typedef struct vtxprim_t
{
	vec3 pos;
	vec3 norm;
	vec2 uv;
} vtxprim_t;