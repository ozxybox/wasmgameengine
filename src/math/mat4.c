#include "linalg_p.h"

#include <math.h>

#if !LINALG_NOSIMD
#include <xmmintrin.h>
#endif

// Implementation of 4 column matrices 


// Matrix 4x4 //

void identity4x4(mat4x4* out)
{
    *out = (mat4x4)
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
    m.a.x = dotv4_p(&row, &r->a);
    m.b.x = dotv4_p(&row, &r->b);
    m.c.x = dotv4_p(&row, &r->c);
    m.d.x = dotv4_p(&row, &r->d);

    row = (vec4){l->a.y, l->b.y, l->c.y, l->d.y};
    m.a.y = dotv4_p(&row, &r->a);
    m.b.y = dotv4_p(&row, &r->b);
    m.c.y = dotv4_p(&row, &r->c);
    m.d.y = dotv4_p(&row, &r->d);

    row = (vec4){l->a.z, l->b.z, l->c.z, l->d.z};
    m.a.z = dotv4_p(&row, &r->a);
    m.b.z = dotv4_p(&row, &r->b);
    m.c.z = dotv4_p(&row, &r->c);
    m.d.z = dotv4_p(&row, &r->d);
    
    row = (vec4){l->a.w, l->b.w, l->c.w, l->d.w};
    m.a.w = dotv4_p(&row, &r->a);
    m.b.w = dotv4_p(&row, &r->b);
    m.c.w = dotv4_p(&row, &r->c);
    m.d.w = dotv4_p(&row, &r->d);

    *out = m;
}

#if LINALG_NOSIMD
void mul4x4v4(vec4* out, mat4x4* left, vec4* right)
{
    // Unrolled
    vec4 v;
    v = scalev4(left->a, right->x);
    v = addv4(v, scalev4(left->b, right->y));
    v = addv4(v, scalev4(left->c, right->z));
    v = addv4(v, scalev4(left->d, right->w));

    *out = v;
}

void scale4x4v4(mat4x4* out, mat4x4* mat, vec4* scale)
{
    out->a = (vec4){mat->a.x * scale->x, mat->a.y * scale->x, mat->a.z * scale->x, mat->a.w * scale->x};
    out->b = (vec4){mat->b.x * scale->y, mat->b.y * scale->y, mat->b.z * scale->y, mat->b.w * scale->y};
    out->c = (vec4){mat->c.x * scale->z, mat->c.y * scale->z, mat->c.z * scale->z, mat->c.w * scale->z};
    out->d = (vec4){mat->d.x * scale->w, mat->d.y * scale->w, mat->d.z * scale->w, mat->d.w * scale->w};
}

#else
void mul4x4v4(vec4* out, mat4x4* left, vec4* right)
{
    // Unrolled
    __m128 v = *(__m128*)right;
    __m128 x, y, z, w;

    // TODO: perf check this against _mm_set1_ps
    x = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
    y = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
    z = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
    w = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));

    *(__m128*)out =
    _mm_add_ps(
        _mm_add_ps(
            _mm_mul_ps(x, *(__m128*)&left->a), 
            _mm_mul_ps(y, *(__m128*)&left->b)),
        _mm_add_ps(
            _mm_mul_ps(z, *(__m128*)&left->c),
            _mm_mul_ps(w, *(__m128*)&left->d))
    );
}

void scale4x4v4(mat4x4* out, mat4x4* mat, vec4* scale)
{
    __m128 v = *(__m128*)scale;
    __m128 x, y, z, w;

    // TODO: perf check this against _mm_set1_ps
    x = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
    y = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
    z = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
    w = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));

    *(__m128*)&out->a = _mm_mul_ps(x, *(__m128*)&mat->a);
    *(__m128*)&out->b = _mm_mul_ps(y, *(__m128*)&mat->b);
    *(__m128*)&out->c = _mm_mul_ps(z, *(__m128*)&mat->c);
    *(__m128*)&out->d = _mm_mul_ps(w, *(__m128*)&mat->d);
}

#endif


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


// Matrix 3x4 //

void identity3x4(mat3x4* mat)
{
    *mat = (mat3x4)
    {{1.0f, 0.0f, 0.0f},
     {0.0f, 1.0f, 0.0f},
     {0.0f, 0.0f, 1.0f},
     {0.0f, 0.0f, 0.0f}};
}