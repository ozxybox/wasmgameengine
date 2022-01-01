#pragma once

// Matrices are in Column-major

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

// Geometry
typedef struct {
    vec3 a, b, c;    
} tri_t;
typedef vec3f trif_t[3];

typedef struct {
    vec3 mins, maxs;    
} aabb_t;

typedef struct {
    vec3 origin;
    float radius;    
} sphere_t;


// Matrix operations
void identity3x4(mat3x4* mat);

void identity4x4(mat4x4* mat);
void mul4x4(mat4x4* out, mat4x4* l, mat4x4* r);
void mul4x4v4(vec4* out, mat4x4* left, vec4* right);
void scale4x4v4(mat4x4* out, mat4x4* mat, vec4* scale);
void rotateXAxis4x4(mat4x4* out, float angle);
void rotateYAxis4x4(mat4x4* out, float angle);
void rotateZAxis4x4(mat4x4* out, float angle);
void rotateXYZ4x4(mat4x4* out, float x, float y, float z);

// Vector 3 functions
float dotv3(vec3 a, vec3 b);
vec3 crossv3(vec3 a, vec3 b);
vec3 addv3(vec3 a, vec3 b);
vec3 subv3(vec3 a, vec3 b);
vec3 addfv3(vec3 a, float b);
vec3 subfv3(vec3 a, float b);
vec3 scalev3(vec3 a, float b);
vec3 normalizev3(vec3 a);
float magv3(vec3 a);

// Vector 4 functions
float dotv4(vec4 a, vec4 b);


// Camera projection functions

void perspective4x4(mat4x4* out, float fov, float near, float far, float aspect);
