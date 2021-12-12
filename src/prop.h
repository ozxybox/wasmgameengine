#pragma once
#include "mesh.h"
#include "texture.h"
#include "shader.h"

#define MAX_PROPS 64
#define PROP_INVALID_INDEX 0xFFFFFFFF 
typedef unsigned int prop_t;

prop_t createProp();
void deleteProp(prop_t prop);
void deleteAllProps();

void setPropInfo(prop_t prop, mesh_t mesh, texture_t texture, shader_t shader);

void setPropOrigin(prop_t prop, vec3 origin);
void setPropRotation(prop_t prop, vec3 rotation);
void setPropScale(prop_t prop, vec3 scale);

vec3 getPropOrigin(prop_t prop);

void setPropVisible(prop_t prop, int visible);
int getPropVisible(prop_t prop);

unsigned int propCount();
void drawProp(prop_t prop);
void drawAllProps();