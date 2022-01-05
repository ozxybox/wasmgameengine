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
    SHADER_UNIFORM_COLOR,
};

#define SHADER_INVALID_INDEX 0xFFFFFFFF 
typedef unsigned int shader_t;

void shader_systemInit();
void shader_systemShutdown();

shader_t shader_create(const char* vs, const char* fs);
shader_t shader_empty();
void shader_delete(shader_t shader);
void shader_loadInto(shader_t empty, const char* vs, const char* fs);

void shader_bind(shader_t shader);
void shader_set(int uniform, void* data);

