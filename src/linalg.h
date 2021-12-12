#pragma once

// User friendly
typedef struct {
    float x,y;
} vec2;
typedef struct {
    float x,y,z;
} vec3;
typedef struct {
    float x,y,z,w;
} vec4;

typedef struct {
    vec3 a,b,c,d;
} mat3x4;
typedef struct {
    vec4 a,b,c,d;
} mat4x4;

// Loop friendly
typedef float vec2f[3];
typedef float vec3f[3];
typedef float vec4f[4];

typedef vec3f mat3x4f[4];
typedef vec4f mat4x4f[4];

// Matrix operations
void identity3x4(mat3x4* mat);

void identity4x4(mat4x4* mat);
void mul4x4(mat4x4* matOut, mat4x4* matA, mat4x4* matB);
void mul4x4v4(vec4* vecOut, mat4x4* matA, vec4* vecB);
void scale4x4v4(mat4x4* out, mat4x4* mat, vec4* scale);
void rotateXAxis4x4(mat4x4* out, float angle);
void rotateYAxis4x4(mat4x4* out, float angle);
void rotateZAxis4x4(mat4x4* out, float angle);
void rotateXYZ4x4(mat4x4* out, float x, float y, float z);

// Vector functions
float dotv3(vec3 a, vec3 b);
vec3 crossv3(vec3 a, vec3 b);
vec3 addv3(vec3 a, vec3 b);
vec3 subv3(vec3 a, vec3 b);
vec3 addfv3(vec3 a, float b);
vec3 subfv3(vec3 a, float b);
vec3 scalev3(vec3 a, float b);

// Camera projection functions

void perspective4x4(mat4x4* out, float fov, float near, float far, float aspect);
