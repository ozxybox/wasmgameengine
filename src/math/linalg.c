#include "linalg.h"
#include <math.h>

// Nonstandard?
#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif


// Matrix operations
void identity2x2(mat2x2* out) {
    *out = (mat2x2)
    {{1.0f, 0.0f},
     {0.0f, 1.0f}};
}
void rotate2x2(mat2x2* out, float angle) {
    *out = (mat2x2)
    {{cosf(angle), -sinf(angle)},
     {sinf(angle),  cosf(angle)}};
}

void rotate2x3(mat2x3* out, float angle) {
    *out = (mat2x3)
    {{cosf(angle), -sinf(angle)},
     {sinf(angle),  cosf(angle)},
     {       0.0f,          0.0f,}};
}

void identity2x3(mat2x3* out) {
    *out = (mat2x3)
    {{1.0f, 0.0f},
     {0.0f, 1.0f},
     {0.0f, 0.0f}};
}
void identity3x3(mat3x3* out) {
    *out = (mat3x3)
    {{1.0f, 0.0f, 0.0f},
     {0.0f, 1.0f, 0.0f},
     {0.0f, 0.0f, 1.0f}};
}


// Vector 2 functions
vec2 addv2(vec2 a, vec2 b) {
    return (vec2){a.x+b.x,a.y+b.y};
}
vec2 subv2(vec2 a, vec2 b) {
    return (vec2){a.x-b.x,a.y-b.y};
}
vec2 addfv2(vec2 a, float b) {
    return (vec2){a.x+b,a.y+b};
}
vec2 subfv2(vec2 a, float b) {
    return (vec2){a.x-b,a.y-b};
}
vec2 scalev2(vec2 a, float b) {
    return (vec2){a.x*b,a.y*b};
}
vec2 normalizev2(vec2 a) {
    float mag = sqrt(a.x*a.x + a.y*a.y);
    return (vec2){a.x / mag, a.y / mag};
}
float dotv2(vec2 a, vec2 b) {
    return a.x*b.x + a.x*b.x;
}
float magv2(vec2 a) {
    return sqrt(a.x*a.x + a.y*a.y);
}


// Vector 3 functions

vec3 crossv3(vec3 a, vec3 b) {
    return (vec3){a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};
}
vec3 addv3(vec3 a, vec3 b) {
    return (vec3){a.x+b.x,a.y+b.y,a.z+b.z};
}
vec3 subv3(vec3 a, vec3 b) {
    return (vec3){a.x-b.x,a.y-b.y,a.z-b.z};
}
vec3 addfv3(vec3 a, float b) {
    return (vec3){a.x+b,a.y+b,a.z+b};
}
vec3 subfv3(vec3 a, float b) {
    return (vec3){a.x-b,a.y-b,a.z-b};
}
vec3 scalev3(vec3 a, float b) {
    return (vec3){a.x*b,a.y*b,a.z*b};
}
vec3 normalizev3(vec3 a) {
    float mag = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
    return (vec3){a.x / mag, a.y / mag, a.z / mag};
}
float dotv3(vec3 a, vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}
float magv3(vec3 a) {
    return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

// Vec4 functions
vec4 addv4(vec4 a, vec4 b) {
    return (vec4){a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w};
}
vec4 subv4(vec4 a, vec4 b) {
    return (vec4){a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w};
}
vec4 addfv4(vec4 a, float b) {
    return (vec4){a.x+b,a.y+b,a.z+b,a.w+b};
}
vec4 subfv4(vec4 a, float b) {
    return (vec4){a.x-b,a.y-b,a.z-b,a.w-b};
}
vec4 scalev4(vec4 a, float b) {
    return (vec4){a.x*b,a.y*b,a.z*b,a.w*b};
}
vec4 normalizev4(vec4 a) {
    float mag = sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
    return (vec4){a.x / mag, a.y / mag, a.z / mag, a.w / mag};
}
float dotv4(vec4 a, vec4 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}
float magv4(vec4 a) {
    return sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
}

// Camera projection functions

void perspective4x4(mat4x4* out, float fov, float near, float far, float aspect) {
    float s = 1/(tanf((fov/2)*(M_PI/180)));

    *out = (mat4x4){
    {s * aspect, 0,                      0,  0},
    {         0, s,                      0,  0},
    {         0, 0,        -far/(far-near), -1},
    {         0, 0, -(far*near)/(far-near),  0}};
}

void ortho4x4(mat4x4* out, float l, float r, float t, float b, float near, float far) {
	*out = (mat4x4){
    {  2.0f/(r-l),            0,                      0, 0},
    {           0,   2.0f/(t-b),                      0, 0},
    {           0,            0,       -2.0f/(far-near), 0},
    {-(r+l)/(r-l), -(t+b)/(t-b), -(far+near)/(far-near), 1}};
}


// Ease of use functions

vec2 rotatev2(vec2 a, float angle) {
    vec2 b;
    b.x =  cosf(angle) * a.x + sinf(angle) * a.y;
    b.y = -sinf(angle) * a.x + cosf(angle) * a.y;
    return b;
}