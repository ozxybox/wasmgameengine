#pragma once

enum SHADER_UNIFORM
{
    SHADER_UNIFORM_MODEL,
    SHADER_UNIFORM_VIEW,
    SHADER_UNIFORM_PROJECTION,
    SHADER_UNIFORM_CAMERAPOS,
    SHADER_UNIFORM_LIGHTPOS,
    SHADER_UNIFORM_LIGHTCOLOR,
    SHADER_UNIFORM_SUNDIR,
};

#define SHADER_INVALID_INDEX 0xFFFFFFFF 
typedef unsigned int shader_t;

shader_t createShader(const char* vs, const char* fs);
shader_t emptyShader();
void loadIntoShader(shader_t empty, const char* vs, const char* fs);

void bindShader(shader_t shader);
void setUniform(int uniform, void* data);

void shaderSystemInit();
