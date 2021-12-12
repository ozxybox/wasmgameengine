#include <emscripten.h> // For emscripten_get_device_pixel_ratio()
#include <emscripten/html5.h> // For Emscripten HTML5 WebGL context creation API
#include <ogl.h>
#include <string.h> // For NULL and strcmp()
#include <assert.h> // For assert()
#include <math.h>

#include "log.h"
#include "mesh.h"
#include "primitives.h"
#include "texture.h"
#include "asset.h"
#include "linalg.h"
#include "shader.h"
#include "transform.h"
#include "camera.h"
#include "renderer.h"
#include "prop.h"

#define MAX_PLAYERS 10
static prop_t s_players[MAX_PLAYERS];
static int s_activePlayers = 0;
static int s_mePlayer = 0;

EMSCRIPTEN_KEEPALIVE
void js_setPlayerPos(int player, float x, float y, float z)
{
    setPropOrigin(s_players[player], (vec3){x,y,z});
}

EMSCRIPTEN_KEEPALIVE
void js_setPlayerActive(int player, int active)
{
    setPropVisible(s_players[player], active);
}


EMSCRIPTEN_KEEPALIVE
void js_setMePlayer(int player)
{
    s_mePlayer = player;
}

static EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext;
static float pixelWidth, pixelHeight;

static mesh_t s_testMesh;
static texture_t s_texBackground;
static texture_t s_texFloor;
static texture_t s_texPawn;
static asset_t* s_poke;
static asset_t* s_shaderAsset;

static shader_t s_shader;
static shader_t s_sky;


typedef void (*tick_func)(double t, double dt);

static int tick(double time, void *userData)
{
    static double t0;
    double dt = time - t0;
    t0 = time;
    tick_func f = (tick_func)(userData);
    f(time, dt);
    return 1;
}


static void texrect(mat4x4* model, texture_t texture, mesh_t m)
{
    //float mat[16] = { (x1-x0)*pixelWidth, 0, 0, 0, 0, (y1-y0)*pixelHeight, 0, 0, 0, 0, 1, 0, x0*pixelWidth-1.f, y0*pixelHeight-1.f, 0, 1};
    //glUniformMatrix4fv(matPos, 1, 0, mat);
    setUniform(SHADER_UNIFORM_MODEL, model);
   

    bindTexture(texture);
    
    bindMesh(m);
    drawMesh(m);
    unbindMesh();

}


static prop_t s_donut;

#include <emscripten.h>

#define WIDTH 1024
#define HEIGHT 768

static texture_t s_donutTex;
static mesh_t s_donutMesh;
// Per-frame animation tick.
int draw_frame(double t, void * p)
{
    preloadSystemProcess();
    clearScreen(0.1f, 0.2f, 0.3f, 1.f);

    float s = t / 1000.0f;


    bindShader(s_sky);

    prop_t me = s_players[s_mePlayer];
    vec3 origin = getPropOrigin(me);
// Set up camera
    camera_t cam;
    //cam.origin = (vec3){2*sinf(s),0,2*cosf(s)};
    //cam.rotation = (vec3){0,-s,0};
    cam.origin = addv3(origin, (vec3){0,2,2});
    cam.rotation = (vec3){3.14/8.0,0,0};
    cam.fov = 65;
    cam.near = 0.1f;
    cam.far = 200.0f;
    cam.aspect = 768.0f/1024.0f;
    applyCamera(&cam);

    // skybox
    {
        mat4x4 model;
        identity4x4(&model);
        vec4 scale = {28, 28, 28, 1};
        scale4x4v4(&model, &model, &scale);
        model.d = (vec4){cam.origin.x,cam.origin.y,cam.origin.z,1};
        texrect(&model, s_texPawn, skyMesh());
    }

    // ground
    {
        mat4x4 model;
        identity4x4(&model);
        vec4 scale = {8, 1, 8, 1};
        scale4x4v4(&model, &model, &scale);
        model.d = (vec4){0,-1.5,0,1};
        texrect(&model, s_texBackground, cubeMesh());
    }


    bindShader(s_shader);

    float sunang = cos(s)*3.14*0.3;
    vec3 sundir = {sin(3.14/4.0),-cos(3.14/4.0),0};
    vec4 lightcolor = {1,1,1,0.45};
    setUniform(SHADER_UNIFORM_SUNDIR, &sundir);
    setUniform(SHADER_UNIFORM_LIGHTCOLOR, &lightcolor);
    
    setPropRotation(0,(vec3){0,s,0});
    setPropRotation(s_donut, (vec3){3.14f * -0.35f * cosf(s), s, 3.14f * -0.15f * sinf(s )});

    drawAllProps();

    return 1;
}

int main()
{
    initGL("canvas", WIDTH, HEIGHT);

    s_testMesh = errorMesh();
    logInfo("cube at %u", s_testMesh);
    s_texBackground = preloadTexture("floral.png");
    s_texFloor = preloadTexture("penguin_body_uv_grid_3.png");
    s_texPawn = preloadTexture("sky.png");
    s_poke = preloadAsset(ASSET_CLASS_MODEL, "penguin-2.obj");

    s_donutTex = preloadTexture("donut_base.png");
    s_donutMesh = preloadAsset(ASSET_CLASS_MODEL, "donut.obj")->data;

    s_sky = 0;

    s_shaderAsset = preloadAsset(ASSET_CLASS_SHADER, "src/phong.glsl");
    s_shader = s_shaderAsset->data; 
    logInfo("Phong %d", s_shader);

    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        s_players[i] = createProp();
        setPropInfo(s_players[i], s_poke->data, s_texFloor, s_shader);
        setPropVisible(s_players[i], 0);
        setPropScale(s_players[i], (vec3){2,2,2});
    }

    setPropVisible(s_players[0], 1);

    EM_ASM(
        superActive = 1;

    );

    // donut prop
    s_donut = createProp();
    setPropInfo(s_donut, s_donutMesh, s_donutTex, s_shader);
    setPropOrigin(s_donut, (vec3){3,2,3});
    setPropScale(s_donut, (vec3){10,10,10});
    
    emscripten_request_animation_frame_loop(&draw_frame, 0);
}

