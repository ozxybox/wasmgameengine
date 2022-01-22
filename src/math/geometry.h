#include "linalg.h"

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