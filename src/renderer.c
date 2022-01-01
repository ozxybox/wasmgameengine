#include "renderer.h"


#include "log.h"
#include "mesh.h"
#include "primitives.h"
#include "texture.h"
#include "asset.h"
#include "linalg.h"
#include "shader.h"
#include "transform.h"
#include "camera.h"

#include <emscripten.h> // For emscripten_get_device_pixel_ratio()
#include <emscripten/html5.h> // For Emscripten HTML5 WebGL context creation API
#include <ogl.h>
#include <string.h> // For NULL and strcmp()
#include <assert.h> // For assert()
#include <math.h>

static EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext;
static float pixelWidth, pixelHeight;


void initGL(const char* canvas, int width, int height)
{
    logMsg("Initializing webgl\n");

    double dpr = emscripten_get_device_pixel_ratio();
    emscripten_set_element_css_size(canvas, width / dpr, height / dpr);
    emscripten_set_canvas_element_size(canvas, width, height);

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.alpha = 0;
    attrs.majorVersion = 2;

    glContext = emscripten_webgl_create_context(canvas, &attrs);
    assert(glContext);
    emscripten_webgl_make_context_current(glContext);

    pixelWidth = 2.f / width;
    pixelHeight = 2.f / height;

    // Systems Init
    asset_systemInit();
    mesh_systemInit();
    primitive_systemInit();
    texture_systemInit();
    shader_systemInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); 
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void clearScreen(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

