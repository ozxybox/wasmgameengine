#pragma once
#include "mesh.h"
#include "material.h"

#define MAX_MODELS 64
#define MODEL_INVALID_INDEX 0xFFFFFFFF 
typedef unsigned int model_t;


typedef struct modelPart_t
{
    mesh_t mesh;
    material_t material;
} modelPart_t;

typedef struct modelData_t
{
    unsigned int partCount;
    modelPart_t* parts;
} modelData_t;


// BIND SHADER BASED ON MATERIAL

