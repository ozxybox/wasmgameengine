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
} propData_t;

propData_t s_props[MAX_PROPS];
unsigned int s_propCount = 0;

propData_t* newProp()
{
    if(s_propCount >= MAX_PROPS) return 0;
    return &s_props[s_propCount++];
}

prop_t createProp()
{
    // New prop
    propData_t* dat = newProp();
    if(!dat) return PROP_INVALID_INDEX;
    dat->active = 1;
    dat->visible = 1;
    dat->transform = (transform_t){{0,0,0},{0,0,0},{1,1,1}};
    dat->mesh = MESH_INVALID_INDEX;
    dat->texture = TEXTURE_INVALID_INDEX;
    dat->shader = SHADER_INVALID_INDEX;
    return dat - s_props;
}

void deleteProp(prop_t prop)
{
    if(prop >= s_propCount) return;
    if(prop + 1 == s_propCount) s_propCount--;
    s_props[prop].active = 0;
}

void deleteAllProps(prop_t prop)
{
    memset(s_props, 0, sizeof(s_props));
    s_propCount = 0;
}

void setPropMesh(prop_t prop, mesh_t mesh);
void setPropTexture(prop_t prop, texture_t texture);
void setPropShader(prop_t prop, shader_t shader);

mesh_t getPropMesh(prop_t prop);
texture_t getPropTexture(prop_t prop);
shader_t getPropShader(prop_t prop);

void setPropInfo(prop_t prop, mesh_t mesh, texture_t texture, shader_t shader)
{
    if(prop >= s_propCount) return;
    s_props[prop].mesh = mesh;
    s_props[prop].texture = texture;
    s_props[prop].shader = shader;
}


void setPropOrigin(prop_t prop, vec3 origin)
{
    if(prop >= s_propCount) return;
    s_props[prop].transform.origin = origin;
}
void setPropRotation(prop_t prop, vec3 rotation)
{
    if(prop >= s_propCount) return;
    s_props[prop].transform.rotation = rotation;
}
void setPropScale(prop_t prop, vec3 scale)
{
    //logInfo("p%d - %f %f %f", prop, scale.x, scale.y, scale.z);
    if(prop >= s_propCount) return;
    s_props[prop].transform.scale = scale;
}

void setPropVisible(prop_t prop, int visible)
{
    if(prop >= s_propCount) return;
    s_props[prop].visible = visible;
}
int getPropVisible(prop_t prop)
{
    if(prop >= s_propCount) return 0;
    return s_props[prop].visible;
}


unsigned int propCount()
{
    return s_propCount;
}

void drawPropData(propData_t* data)
{
    if(!data->visible || !data->active)
        return;

    //logInfo("Shader %d mesh %d texture %d", data->shader, data->mesh, data->texture);
    
    // Shader
    bindShader(data->shader);
    
    // Texture
    bindTexture(data->texture);
    
    // Transformation
    mat4x4 model;
    transformMatrix(&data->transform, &model);
    setUniform(SHADER_UNIFORM_MODEL, &model);
    //logInfo("p%d - %f %f %f", data - s_props, data->transform.scale.x, data->transform.scale.y, data->transform.scale.z);
    
    // Draw mesh
    bindMesh(data->mesh);
    drawMesh(data->mesh);
    unbindMesh();
}

void drawProp(prop_t prop)
{
    if(prop >= s_propCount) return;
    drawPropData(&s_props[prop]);
}

void drawAllProps()
{
    for(unsigned int i = 0; i < s_propCount; i++)
    {
        drawPropData(&s_props[i]);
    }
}

vec3 getPropOrigin(prop_t prop)
{
    return s_props[prop].transform.origin;
}