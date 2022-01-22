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

typedef struct {
    vec3 origin, dir;
} ray_t;

int rayPlaneTest(ray_t ray, vec3 normal, vec3 planePoint, vec3* poi, float* closestT);
int rayTriangleTest(ray_t ray, tri_t tri, vec3 norm, vec3* poi, float* closestT);
int sphereTriangleTest(float radius, vec3 origin, tri_t tri, vec3 norm, vec3* clip);