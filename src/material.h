#pragma once
#include "texture.h"
#include "shader.h"

typedef struct {
    shader_t shader;
    texture_t diffuseTexture;
} material_t;
