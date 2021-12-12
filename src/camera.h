#pragma once
#include "linalg.h"

typedef struct camera_t
{
    vec3 origin;
    vec3 rotation;

    float fov;
    float near;
    float far;
    float aspect;
} camera_t;

void applyCamera(camera_t* cam);