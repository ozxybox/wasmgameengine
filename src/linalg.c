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

void mul4x4(mat4x4* matOut, mat4x4* matA, mat4x4* matB)
{
    mat4x4f* a   = (mat4x4f*)matA;
    mat4x4f* b   = (mat4x4f*)matB;
    mat4x4f  out;

    for(int m = 0; m < 4; m++)
    {
        for(int n = 0; n < 4; n++)
        {
            float sum = 0;
            for(int i = 0; i < 4; i++)
                sum += (*a)[m][i] * (*b)[i][n];
            out[m][n] = sum;
        }
    }

    *matOut = *(mat4x4*)&out;
}

void mul4x4v4(vec4* vecOut, mat4x4* matA, vec4* vecB)
{
    mat4x4f* a   = (mat4x4f*)matA;
    vec4f*   b   = (vec4f*)  vecB;
    vec4f    out;

    for(int m = 0; m < 4; m++)
    {
        float sum = 0;
        for(int i = 0; i < 4; i++)
            sum += (*a)[m][i] * (*b)[i];
        out[m] = sum;
    }

    *vecOut = *(vec4*)&out;
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
    rotateXAxis4x4(&mat, x); // Pitch
    rotateYAxis4x4(&rot, y); // Yaw
    mul4x4(&mat, &rot, &mat);
    rotateZAxis4x4(&rot, z); // Roll
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
