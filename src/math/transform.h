#pragma once
#include "linalg.h"

typedef struct transform_t
{
    vec3 origin;
    vec3 rotation;
    vec3 scale;
} transform_t;

transform_t transform();
void transform_toMatrix(transform_t* transform, mat4x4* matrix); // Pitch, yaw, roll