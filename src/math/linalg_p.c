#include "linalg_p.h"
#include <math.h>

#if !LINALG_NOSIMD
#include <xmmintrin.h>
#endif


// Vector 2 //
void addv2_p(vec2* o, vec2* b) {
    o->x += b->x;
    o->y += b->y;
}
void subv2_p(vec2* o, vec2* b) {
    o->x -= b->x;
    o->y -= b->y;
}
void addfv2_p(vec2* o, float b) {
    o->x += b;
    o->y += b;
}
void subfv2_p(vec2* o, float b) {
    o->x -= b;
    o->y -= b;
}
void scalev2_p(vec2* o, float b) {
    o->x *= b;
    o->y *= b;
}
void normalizev2_p(vec2* o) {
    float mag = sqrt(o->x*o->x + o->y*o->y);
    o->x /= mag;
    o->y /= mag;
}
float dotv2_p(vec2* o, vec2* b) {
    return o->x*b->x + o->y*b->y;
}
float magv2_p(vec2* o) {
    return sqrt(o->x*o->x + o->y*o->y);
}

// Vector 3 //
void crossv3_p(vec3* o, vec3* b) {
    vec3 v;
    v.x = o->y*b->z - o->z*b->y;
    v.y = o->z*b->x - o->x*b->z;
    v.z = o->x*b->y - o->y*b->x;
    *o = v;
}
void addv3_p(vec3* o, vec3* b) {
    o->x += b->x;
    o->y += b->y;
    o->z += b->z;
}
void subv3_p(vec3* o, vec3* b) {
    o->x -= b->x;
    o->y -= b->y;
    o->z -= b->z;
}
void addfv3_p(vec3* o, float b) {
    o->x += b;
    o->y += b;
    o->z += b;
}
void subfv3_p(vec3* o, float b) {
    o->x -= b;
    o->y -= b;
    o->z -= b;
}
void scalev3_p(vec3* o, float b) {
    o->x *= b;
    o->y *= b;
    o->z *= b;
}
void normalizev3_p(vec3* o) {
    float mag = sqrt(o->x*o->x + o->y*o->y + o->z*o->z);

    o->x /= mag;
    o->y /= mag;
    o->z /= mag;
}
float dotv3_p(vec3* o, vec3* b) {
    return o->x*b->x + o->y*b->y + o->z*b->z;
}
float magv3_p(vec3* o) {
    return sqrt(o->x*o->x + o->y*o->y + o->z*o->z);
}

#if LINALG_NOSIMD


// Vector 4 //
void addv4_p(vec4* o, vec4* b) {
    o->x += b->x;
    o->y += b->y;
    o->z += b->z;
    o->w += b->w;
}
void subv4_p(vec4* o, vec4* b) {
    o->x -= b->x;
    o->y -= b->y;
    o->z -= b->z;
    o->w -= b->w;
}
void addfv4_p(vec4* o, float b) {
    o->x += b;
    o->y += b;
    o->z += b;
    o->w += b;
}
void subfv4_p(vec4* o, float b) {
    o->x -= b;
    o->y -= b;
    o->z -= b;
    o->w -= b;
}
void scalev4_p(vec4* o, float b) {
    o->x *= b;
    o->y *= b;
    o->z *= b;
    o->w *= b;
}
void normalizev4_p(vec4* o) {
    float mag = sqrt(o->x*o->x + o->y*o->y + o->z*o->z + o->w*o->w);

    o->x /= mag;
    o->y /= mag;
    o->z /= mag;
    o->w /= mag;
}
float dotv4_p(vec4* o, vec4* b) {
    return o->x*b->x + o->y*b->y + o->z*b->z + o->w*b->w;
}
float magv4_p(vec4* o) {
    return sqrt(o->x*o->x + o->y*o->y + o->z*o->z + o->w*o->w);
}
#else


// Vector 4 //
void addv4_p(vec4* o, vec4* b) {
    *(__m128*)o = _mm_add_ps(*(__m128*)o, *(__m128*)b);
}
void subv4_p(vec4* o, vec4* b) {
    *(__m128*)o = _mm_sub_ps(*(__m128*)o, *(__m128*)b);
}
void addfv4_p(vec4* o, float b) {
    *(__m128*)o = _mm_add_ps(*(__m128*)o, _mm_set_ps1(b));
}
void subfv4_p(vec4* o, float b) {
    *(__m128*)o = _mm_sub_ps(*(__m128*)o, _mm_set_ps1(b));
}
void scalev4_p(vec4* o, float b) {
    *(__m128*)o = _mm_mul_ps(*(__m128*)o, _mm_set_ps1(b));
}
void normalizev4_p(vec4* o) {
    __m128 s, d = _mm_mul_ps(*(__m128*)o, *(__m128*)o);
    s = _mm_shuffle_ps(d, d, _MM_SHUFFLE(2, 3, 0, 1));
    d = _mm_add_ps(d, s);
    s = _mm_movehl_ps(s, d);
    d = _mm_add_ss(d, s);
    d = _mm_rsqrt_ss(d);
    d = _mm_shuffle_ps(d, d, 0);

    *(__m128*)o = _mm_mul_ps(*(__m128*)o, d);
}
float dotv4_p(vec4* o, vec4* b) {
    __m128 s, d = _mm_mul_ps(*(__m128*)o, *(__m128*)b);
    s = _mm_shuffle_ps(d, d, _MM_SHUFFLE(2, 3, 0, 1));
    d = _mm_add_ps(d, s);
    s = _mm_movehl_ps(s, d);
    d = _mm_add_ss(d, s);
    return *(float*)&d;
}
float magv4_p(vec4* o) {
    __m128 s, d = _mm_mul_ps(*(__m128*)o, *(__m128*)o);
    s = _mm_shuffle_ps(d, d, _MM_SHUFFLE(2, 3, 0, 1));
    d = _mm_add_ps(d, s);
    s = _mm_movehl_ps(s, d);
    d = _mm_add_ss(d, s);
    d = _mm_rsqrt_ss(d);
    return *(float*)&d;
}

#endif