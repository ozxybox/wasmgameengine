#pragma once
#include "mesh.h"
#include "texture.h"
#include "shader.h"

#define MAX_PROPS 64
#define PROP_INVALID_INDEX 0xFFFFFFFF 
typedef unsigned int prop_t;

prop_t prop_create();
void prop_delete(prop_t prop);
void prop_deleteAll();

void prop_setInfo(prop_t prop, mesh_t mesh, texture_t texture, shader_t shader);

void prop_setOrigin(prop_t prop, vec3 origin);
void prop_setRotation(prop_t prop, vec3 rotation);
void prop_setScale(prop_t prop, vec3 scale);

vec3 prop_getOrigin(prop_t prop);

void prop_setVisible(prop_t prop, int visible);
int prop_getVisible(prop_t prop);

void prop_setColor(prop_t prop, vec3 color);


unsigned int prop_count();
void prop_draw(prop_t prop);
void prop_drawAll();