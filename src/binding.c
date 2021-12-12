#include <emscripten/emscripten.h>

#include "renderer.h"
#include "asset.h"
#include "model.h"
#include "shader.h"
#include "texture.h"
#include "prop.h"
#include "camera.h"

static camera_t s_cam;
    

EMSCRIPTEN_KEEPALIVE
void js_init(const char* context, int width, int height)
{
    initGL(context, width, height);

    s_cam.origin = (vec3){0,0,0};
    s_cam.rotation = (vec3){0,0,0};
    s_cam.fov = 65;
    s_cam.near = 0.1f;
    s_cam.far = 200.0f;
    s_cam.aspect = height/(float)width;
}

EMSCRIPTEN_KEEPALIVE
void js_frame()
{
    preloadSystemProcess();

    clearScreen(0.1f, 0.2f, 0.3f, 1.f);
}

// Asset system
EMSCRIPTEN_KEEPALIVE
mesh_t js_preloadMesh(const char* path)
{
    return preloadAsset(ASSET_CLASS_MODEL, path)->data;
} 

EMSCRIPTEN_KEEPALIVE
texture_t js_preloadTexture(const char* path)
{
    return preloadAsset(ASSET_CLASS_TEXTURE, path)->data;
} 

EMSCRIPTEN_KEEPALIVE
shader_t js_preloadShader(const char* path)
{
    return preloadAsset(ASSET_CLASS_SHADER, path)->data;
} 

// Camera system
EMSCRIPTEN_KEEPALIVE
void js_setCameraOrigin(float x, float y, float z)
{
    s_cam.origin = (vec3){x, y, z};
} 

EMSCRIPTEN_KEEPALIVE
void js_setCameraRotation(float x, float y, float z)
{
    s_cam.rotation = (vec3){x, y, z};
} 

EMSCRIPTEN_KEEPALIVE
void js_applyCamera()
{
    applyCamera(&s_cam);
}

// Shader System
EMSCRIPTEN_KEEPALIVE
void js_setSunDir(float pitch, float yaw, float roll)
{
    // Euler to vector, the slow way
    mat4x4 mat;
    rotateXYZ4x4(&mat, pitch, yaw, roll);
    vec4 dir = {0,0,1,1};
    mul4x4v4(&dir, &mat, &dir);
    vec3 d3 = {dir.x, dir.y, dir.z};
    setUniform(SHADER_UNIFORM_SUNDIR, &d3);
}

EMSCRIPTEN_KEEPALIVE
void js_setSunColor(float r, float g, float b, float ambient)
{
    vec4 lightcolor = {r,g,b,ambient};
    setUniform(SHADER_UNIFORM_LIGHTCOLOR, &lightcolor);
}


// Prop System


EMSCRIPTEN_KEEPALIVE
prop_t js_createProp()
{
    return createProp();
}

EMSCRIPTEN_KEEPALIVE
void js_deleteProp(prop_t prop)
{
    deleteProp(prop);
}

EMSCRIPTEN_KEEPALIVE
void js_deleteAllProps()
{
    deleteAllProps();
}


EMSCRIPTEN_KEEPALIVE
void js_setPropInfo(prop_t prop, mesh_t mesh, texture_t texture, shader_t shader)
{
    setPropInfo(prop, mesh, texture, shader);
}


EMSCRIPTEN_KEEPALIVE
void js_setPropOrigin(prop_t prop, float x, float y, float z)
{
    setPropOrigin(prop, (vec3){x,y,z});
}

EMSCRIPTEN_KEEPALIVE
void js_setPropRotation(prop_t prop, float pitch, float yaw, float roll)
{
    setPropRotation(prop, (vec3){pitch, yaw, roll});
}


EMSCRIPTEN_KEEPALIVE
void js_setPropScale(prop_t prop, float width, float height, float depth)
{
    setPropScale(prop, (vec3){width, height, depth});
}

EMSCRIPTEN_KEEPALIVE
void js_setPropVisible(prop_t prop, int visible)
{
    setPropVisible(prop, visible);
}

EMSCRIPTEN_KEEPALIVE
int js_getPropVisible(prop_t prop)
{
    return getPropVisible(prop);
}

EMSCRIPTEN_KEEPALIVE
unsigned int js_propCount()
{
    return propCount();
}

EMSCRIPTEN_KEEPALIVE
void js_drawProp(prop_t prop)
{
    drawProp(prop);
}

EMSCRIPTEN_KEEPALIVE
void js_drawAllProps()
{
    drawAllProps();
}

/*

#include <emscripten.h>
#include <emscripten/html5.h>

#include <math.h>

#define WIDTH 1024
#define HEIGHT 768

static prop_t s_prop;
// Per-frame animation tick.
int draw_frame(double t, void * p)
{
    js_frame();

    float s = t / 1000.0;

    // Camera
    js_setCameraOrigin(2*sinf(s), 0, 2*cosf(s));
    js_setCameraRotation(0, -s, 0);
    js_applyCamera();

    js_drawAllProps();

    return 1;
}

int main()
{
    js_init("canvas", WIDTH, HEIGHT);
    s_prop = js_createProp();
    js_setPropInfo(s_prop, 0,0,0);

    emscripten_request_animation_frame_loop(&draw_frame, 0);
    return 1;
}
*/