#pragma once

///////////////////////////////////////////////
// Matrices are in Column-major!
// 
// Matrix types are written in n by m form
// Any type postfixed with "f" are float arrays
// 

// Enable or disable SIMD optimized functions
#define LINALG_NOSIMD 1


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
    vec2 a,b;
} mat2x2;
typedef struct {
    vec2 a,b,c;
} mat2x3;
typedef struct {
    vec3 a,b,c;
} mat3x3;
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

typedef vec2f mat2x2f[2];
typedef vec2f mat2x3f[3];
typedef vec3f mat3x3f[3];
typedef vec3f mat3x4f[4];
typedef vec4f mat4x4f[4];


// Matrix 2x2 //
void identity2x2(mat2x2* out);
void rotate2x2(mat2x2* out, float angle);

// Matrix 2x3 //
void identity2x3(mat2x3* out);
void rotate2x3(mat2x3* out, float angle);

// Matrix 3x4 //
void identity3x3(mat3x3* out);
void identity3x4(mat3x4* out);

// Matrix 4x4 //
void identity4x4(mat4x4* out);
void mul4x4(mat4x4* out, mat4x4* l, mat4x4* r);
void mul4x4v4(vec4* out, mat4x4* left, vec4* right);
void scale4x4v4(mat4x4* out, mat4x4* mat, vec4* scale);
void rotateXAxis4x4(mat4x4* out, float angle);
void rotateYAxis4x4(mat4x4* out, float angle);
void rotateZAxis4x4(mat4x4* out, float angle);
void rotateXYZ4x4(mat4x4* out, float x, float y, float z);


// Vector 2 //
vec2 addv2(vec2 a, vec2 b);
vec2 subv2(vec2 a, vec2 b);
vec2 addfv2(vec2 a, float b);
vec2 subfv2(vec2 a, float b);
vec2 scalev2(vec2 a, float b);
vec2 normalizev2(vec2 a);
float dotv2(vec2 a, vec2 b);
float magv2(vec2 a);

// Vector 3 //
vec3 crossv3(vec3 a, vec3 b);
vec3 addv3(vec3 a, vec3 b);
vec3 subv3(vec3 a, vec3 b);
vec3 addfv3(vec3 a, float b);
vec3 subfv3(vec3 a, float b);
vec3 scalev3(vec3 a, float b);
vec3 normalizev3(vec3 a);
float dotv3(vec3 a, vec3 b);
float magv3(vec3 a);

// Vector 4 //
vec4 addv4(vec4 a, vec4 b);
vec4 subv4(vec4 a, vec4 b);
vec4 addfv4(vec4 a, float b);
vec4 subfv4(vec4 a, float b);
vec4 scalev4(vec4 a, float b);
vec4 normalizev4(vec4 a);
float dotv4(vec4 a, vec4 b);
float magv4(vec4 a);


// Camera functions //
void perspective4x4(mat4x4* out, float fov, float near, float far, float aspect);
void ortho4x4(mat4x4* out, float l, float r, float t, float b, float near, float far);


// Ease of use functions //
vec2 rotatev2(vec2 a, float angle);