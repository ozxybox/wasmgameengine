#include <emscripten/emscripten.h>

#include "renderer.h"
#include "asset.h"
#include "model.h"
#include "shader.h"
#include "texture.h"
#include "prop.h"
#include "camera.h"
#include "primitives.h"
#include "physics.h"
#include "log.h"

camera_t s_cam;

EMSCRIPTEN_KEEPALIVE
void js_init(const char* context, int width, int height, float curtime)
{
    initGL(context, width, height);

    s_cam.origin = (vec3){0,0,0};
    s_cam.rotation = (vec3){0,0,0};
    s_cam.fov = 65;
    s_cam.near = 0.1f;
    s_cam.far = 200.0f;
    s_cam.aspect = height/(float)width;

    physics_systemInit(curtime);
}

EMSCRIPTEN_KEEPALIVE
void js_frame(float curtime)
{
    asset_systemThinkSlow();
    //texture_systemThinkSlow();
    physics_frame(curtime);

    clearScreen(0.1f, 0.2f, 0.3f, 1.f);
}

// Asset system
/*
EMSCRIPTEN_KEEPALIVE
mesh_t js_mesh_preload(const char* path)
{
    return asset_preload(ASSET_CLASS_MODEL, path)->id;
} 

EMSCRIPTEN_KEEPALIVE
texture_t js_texture_preload(const char* path)
{
    return asset_preload(ASSET_CLASS_TEXTURE, path)->id;
} 

EMSCRIPTEN_KEEPALIVE
shader_t js_shader_preload(const char* path)
{
    return asset_preload(ASSET_CLASS_SHADER, path)->id;
} 
*/

EMSCRIPTEN_KEEPALIVE
asset_t* js_asset_preload(int assetclass, const char* url)
{
    return asset_preload(assetclass, url);
}

EMSCRIPTEN_KEEPALIVE
void js_asset_incref(asset_t* asset)
{
    if(asset)
        asset_incref(asset);
}

EMSCRIPTEN_KEEPALIVE
void js_asset_decref(asset_t* asset)
{
    if(asset)
        asset_decref(asset);
}

EMSCRIPTEN_KEEPALIVE
assetIndex_t js_asset_indexof(asset_t* asset)
{
    assetIndex_t a = asset_indexof(asset);
    return a;
}

EMSCRIPTEN_KEEPALIVE
asset_t* js_asset_atindex(assetIndex_t index)
{
    return asset_atindex(index);
}

EMSCRIPTEN_KEEPALIVE
unsigned int js_asset_data(asset_t* asset)
{
    return asset->id;
}



// Camera system
EMSCRIPTEN_KEEPALIVE
void js_camera_setOrigin(float x, float y, float z)
{
    s_cam.origin = (vec3){x, y, z};
} 

EMSCRIPTEN_KEEPALIVE
void js_camera_setRotation(float x, float y, float z)
{
    s_cam.rotation = (vec3){x, y, z};
} 

EMSCRIPTEN_KEEPALIVE
void js_camera_apply()
{
    camera_apply(&s_cam);
}

// Shader System
EMSCRIPTEN_KEEPALIVE
void js_sun_setDir(float pitch, float yaw, float roll)
{
    // Euler to vector, the slow way
    mat4x4 mat;
    rotateXYZ4x4(&mat, pitch, yaw, roll);
    vec4 dir = {0,0,-1,1};
    mul4x4v4(&dir, &mat, &dir);
    vec3 d3 = {dir.x, dir.y, dir.z};
    shader_set(SHADER_UNIFORM_SUNDIR, &d3);
}

EMSCRIPTEN_KEEPALIVE
void js_sun_setColor(float r, float g, float b, float ambient)
{
    vec4 lightcolor = {r,g,b,ambient};
    shader_set(SHADER_UNIFORM_LIGHTCOLOR, &lightcolor);
}


// Prop System


EMSCRIPTEN_KEEPALIVE
prop_t js_prop_create()
{
    return prop_create();
}

EMSCRIPTEN_KEEPALIVE
void js_prop_delete(prop_t prop)
{
    prop_delete(prop);
}

EMSCRIPTEN_KEEPALIVE
void js_prop_deleteAll()
{
    prop_deleteAll();
}


EMSCRIPTEN_KEEPALIVE
void js_prop_setInfo(prop_t prop, mesh_t mesh, texture_t texture, shader_t shader)
{
    prop_setInfo(prop, mesh, texture, shader);
}


EMSCRIPTEN_KEEPALIVE
void js_prop_setOrigin(prop_t prop, float x, float y, float z)
{
    prop_setOrigin(prop, (vec3){x,y,z});
}

EMSCRIPTEN_KEEPALIVE
void js_prop_setRotation(prop_t prop, float pitch, float yaw, float roll)
{
    prop_setRotation(prop, (vec3){pitch, yaw, roll});
}


EMSCRIPTEN_KEEPALIVE
void js_prop_setScale(prop_t prop, float width, float height, float depth)
{
    prop_setScale(prop, (vec3){width, height, depth});
}

EMSCRIPTEN_KEEPALIVE
void js_prop_setVisible(prop_t prop, int visible)
{
    prop_setVisible(prop, visible);
}

EMSCRIPTEN_KEEPALIVE
int js_prop_getVisible(prop_t prop)
{
    return prop_getVisible(prop);
}

EMSCRIPTEN_KEEPALIVE
unsigned int js_prop_count()
{
    return prop_count();
}

EMSCRIPTEN_KEEPALIVE
void js_prop_draw(prop_t prop)
{
    prop_draw(prop);
}

EMSCRIPTEN_KEEPALIVE
void js_prop_drawAll()
{
    prop_drawAll();
}

EMSCRIPTEN_KEEPALIVE
void js_prop_setColor(prop_t prop, float red, float green, float blue)
{
    prop_setColor(prop, (vec3){red, green, blue});
}

// Primitives
EMSCRIPTEN_KEEPALIVE
mesh_t js_cubeMesh()
{
    return cubeMesh();
}

EMSCRIPTEN_KEEPALIVE
mesh_t js_skyMesh()
{
    return skyMesh();
}

// Texture System

EMSCRIPTEN_KEEPALIVE
void js_texture_incRef(texture_t tex)
{
    //texture_incRef(tex);
}

EMSCRIPTEN_KEEPALIVE
void js_texture_decRef(texture_t tex)
{
    //texture_decRef(tex);
}

