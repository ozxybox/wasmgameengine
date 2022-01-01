#include "linalg.h"
#include <math.h>

// Nonstandard?
#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

// Matrix operations
void identity3x4(mat3x4* mat)
{
    *mat = (mat3x4)
    {{1.0f, 0.0f, 0.0f},
     {0.0f, 1.0f, 0.0f},
     {0.0f, 0.0f, 1.0f},
     {0.0f, 0.0f, 0.0f}};
}

void identity4x4(mat4x4* mat)
{
    *mat = (mat4x4)
    {{1.0f, 0.0f, 0.0f, 0.0f},
     {0.0f, 1.0f, 0.0f, 0.0f},
     {0.0f, 0.0f, 1.0f, 0.0f},
     {0.0f, 0.0f, 0.0f, 1.0f}};
}

void mul4x4(mat4x4* out, mat4x4* l, mat4x4* r)
{
    // Unrolled
    mat4x4 m;
    vec4 row;

    row = (vec4){l->a.x, l->b.x, l->c.x, l->d.x};
    m.a.x = dotv4(row, r->a);
    m.b.x = dotv4(row, r->b);
    m.c.x = dotv4(row, r->c);
    m.d.x = dotv4(row, r->d);

    row = (vec4){l->a.y, l->b.y, l->c.y, l->d.y};
    m.a.y = dotv4(row, r->a);
    m.b.y = dotv4(row, r->b);
    m.c.y = dotv4(row, r->c);
    m.d.y = dotv4(row, r->d);

    row = (vec4){l->a.z, l->b.z, l->c.z, l->d.z};
    m.a.z = dotv4(row, r->a);
    m.b.z = dotv4(row, r->b);
    m.c.z = dotv4(row, r->c);
    m.d.z = dotv4(row, r->d);
    
    row = (vec4){l->a.w, l->b.w, l->c.w, l->d.w};
    m.a.w = dotv4(row, r->a);
    m.b.w = dotv4(row, r->b);
    m.c.w = dotv4(row, r->c);
    m.d.w = dotv4(row, r->d);

    *out = m;
}

void mul4x4v4(vec4* out, mat4x4* left, vec4* right)
{
    // Unrolled

    vec4 v;
    vec4 r = *right;

    v.x = dotv4((vec4){left->a.x, left->b.x, left->c.x, left->d.x}, r);
    v.y = dotv4((vec4){left->a.y, left->b.y, left->c.y, left->d.y}, r);
    v.z = dotv4((vec4){left->a.z, left->b.z, left->c.z, left->d.z}, r);
    v.w = dotv4((vec4){left->a.w, left->b.w, left->c.w, left->d.w}, r);

    *out = v;
}


void scale4x4v4(mat4x4* out, mat4x4* mat, vec4* scale)
{
    out->a = (vec4){mat->a.x * scale->x, mat->a.y * scale->x, mat->a.z * scale->x, mat->a.w * scale->x};
    out->b = (vec4){mat->b.x * scale->y, mat->b.y * scale->y, mat->b.z * scale->y, mat->b.w * scale->y};
    out->c = (vec4){mat->c.x * scale->z, mat->c.y * scale->z, mat->c.z * scale->z, mat->c.w * scale->z};
    out->d = (vec4){mat->d.x * scale->w, mat->d.y * scale->w, mat->d.z * scale->w, mat->d.w * scale->w};
}

void rotateXAxis4x4(mat4x4* out, float angle)
{
    *out = (mat4x4)
    {{1.0f,        0.0f,         0.0f, 0.0f},
     {0.0f, cosf(angle), -sinf(angle), 0.0f},
     {0.0f, sinf(angle),  cosf(angle), 0.0f},
     {0.0f,        0.0f,         0.0f, 1.0f}};
}

void rotateYAxis4x4(mat4x4* out, float angle)
{
    *out = (mat4x4)
    {{ cosf(angle), 0.0f, sinf(angle), 0.0f},
     {        0.0f, 1.0f,        0.0f, 0.0f},
     {-sinf(angle), 0.0f, cosf(angle), 0.0f},
     {        0.0f, 0.0f,        0.0f, 1.0f}};
}

void rotateZAxis4x4(mat4x4* out, float angle)
{
    *out = (mat4x4)
    {{cosf(angle), -sinf(angle), 0.0f, 0.0f},
     {sinf(angle),  cosf(angle), 0.0f, 0.0f},
     {       0.0f,         0.0f, 1.0f, 0.0f},
     {       0.0f,         0.0f, 0.0f, 1.0f}};
}

void rotateXYZ4x4(mat4x4* out, float x, float y, float z)
{
    // TODO: Optimize this down into one big matrix 
    mat4x4 mat, rot;
    rotateZAxis4x4(&mat, z); // Roll
    rotateXAxis4x4(&rot, x); // Pitch
    mul4x4(&mat, &rot, &mat);
    rotateYAxis4x4(&rot, y); // Yaw
    mul4x4(&mat, &rot, &mat);
    *out = mat;
}
// Vector functions

float dotv3(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 crossv3(vec3 a, vec3 b)
{
    return (vec3){a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};
}

vec3 addv3(vec3 a, vec3 b)
{
    return (vec3){a.x+b.x,a.y+b.y,a.z+b.z};
}

vec3 subv3(vec3 a, vec3 b)
{
    return (vec3){a.x-b.x,a.y-b.y,a.z-b.z};
}

vec3 addfv3(vec3 a, float b)
{
    return (vec3){a.x+b,a.y+b,a.z+b};
}

vec3 subfv3(vec3 a, float b)
{
    return (vec3){a.x-b,a.y-b,a.z-b};
}

vec3 scalev3(vec3 a, float b)
{
    return (vec3){a.x*b,a.y*b,a.z*b};
}

vec3 normalizev3(vec3 a)
{
    float mag = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    return (vec3){a.x / mag, a.y / mag, a.z / mag};
}

// Vec4 functions
float dotv4(vec4 a, vec4 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Camera projection functions

void perspective4x4(mat4x4* out, float fov, float near, float far, float aspect)
{
    float s = 1/(tanf((fov/2)*(M_PI/180)));

    *out = (mat4x4){
    {s * aspect, 0,        0,      0},
    {0, s,        0,      0},
    {0, 0,     -far/(far-near), -1},
    {0, 0, -(far*near)/(far-near),  0}};

}
