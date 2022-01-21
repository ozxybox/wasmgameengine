#include "prop.h"
#include "transform.h"
#include "log.h"
#include <string.h>

typedef struct propData_t {
    int active;
    int visible;
    transform_t transform; 
    mesh_t mesh;
    texture_t texture;
    shader_t shader;
    vec3 color;
} propData_t;

propData_t s_props[MAX_PROPS];
unsigned int s_prop_count = 0;

propData_t* newProp()
{
    if(s_prop_count >= MAX_PROPS) return 0;
    return &s_props[s_prop_count++];
}

prop_t prop_create()
{
    // New prop
    propData_t* dat = newProp();
    if(!dat) return PROP_INVALID_INDEX;
    dat->active = 1;
    dat->visible = 1;
    dat->transform = (transform_t){{0,0,0},{0,0,0},{1,1,1}};
    dat->mesh = MESH_INVALID_INDEX;
    dat->texture = 0;
    dat->shader = SHADER_INVALID_INDEX;
    dat->color = (vec3){1.0f, 1.0f, 1.0f};
    return dat - s_props;
}

void prop_delete(prop_t prop)
{
    if(prop >= s_prop_count) return;
    if(prop + 1 == s_prop_count) s_prop_count--;
    s_props[prop].active = 0;
}

void prop_deleteAll(prop_t prop)
{
    memset(s_props, 0, sizeof(s_props));
    s_prop_count = 0;
}

void setPropMesh(prop_t prop, mesh_t mesh);
void setPropTexture(prop_t prop, texture_t texture);
void setPropShader(prop_t prop, shader_t shader);

mesh_t getPropMesh(prop_t prop);
texture_t getPropTexture(prop_t prop);
shader_t getPropShader(prop_t prop);

void prop_setInfo(prop_t prop, mesh_t mesh, texture_t texture, shader_t shader)
{
    if(prop >= s_prop_count) return;
    s_props[prop].mesh = mesh;
    s_props[prop].texture = texture;
    s_props[prop].shader = shader;
}


void prop_setOrigin(prop_t prop, vec3 origin)
{
    if(prop >= s_prop_count) return;
    s_props[prop].transform.origin = origin;
}
void prop_setRotation(prop_t prop, vec3 rotation)
{
    if(prop >= s_prop_count) return;
    s_props[prop].transform.rotation = rotation;
}
void prop_setScale(prop_t prop, vec3 scale)
{
    //logInfo("p%d - %f %f %f", prop, scale.x, scale.y, scale.z);
    if(prop >= s_prop_count) return;
    s_props[prop].transform.scale = scale;
}


vec3 prop_getOrigin(prop_t prop)
{
    return s_props[prop].transform.origin;
}



void prop_setVisible(prop_t prop, int visible)
{
    if(prop >= s_prop_count) return;
    s_props[prop].visible = visible;
}
int prop_getVisible(prop_t prop)
{
    if(prop >= s_prop_count) return 0;
    return s_props[prop].visible;
}


unsigned int prop_count()
{
    return s_prop_count;
}

void prop_drawData(propData_t* data)
{
    if(!data->visible || !data->active)
        return;

    //logInfo("Shader %d mesh %d texture %d", data->shader, data->mesh, data->texture);
    
    // Shader
    shader_bind(data->shader);
    
    // Texture
    texture_bind(data->texture);
    shader_set(SHADER_UNIFORM_COLOR, &data->color);
    
    // Transformation
    mat4x4 model;
    transform_toMatrix(&data->transform, &model);
    shader_set(SHADER_UNIFORM_MODEL, &model);
    //logInfo("p%d - %f %f %f", data - s_props, data->transform.scale.x, data->transform.scale.y, data->transform.scale.z);
    


    // Draw mesh
    mesh_bind(data->mesh);
    mesh_draw(data->mesh);
    mesh_unbind();
}

void prop_draw(prop_t prop)
{
    if(prop >= s_prop_count) return;
    prop_drawData(&s_props[prop]);
}

void prop_drawAll()
{
    for(unsigned int i = 0; i < s_prop_count; i++)
    {
        prop_drawData(&s_props[i]);
    }
}

void prop_setColor(prop_t prop, vec3 color)
{
    if(prop >= s_prop_count) return;
    s_props[prop].color = color;
}
