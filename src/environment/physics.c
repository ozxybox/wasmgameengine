#include "physics.h"
#include "log.h"
#include "player.h"

// How long is one frame of simulations?
#define PHYSICS_FRAMETIME (1.0f / 60.0f) // 60 ups


typedef struct rigidbodyData_t
{
    float mass;
    vec3 position;
    vec3 velocity;
    vec3 force;
    //vec3 acceleration;
    int onground;
} rigidbodyData_t;

static float s_lastFrameTime;
static float s_curPhysTime;

static vec3 s_physicsGravity;

static rigidbodyData_t s_body;
static rigidbodyData_t s_defbody;
static float s_lastcamy = 1e9;

void physics_systemInit(float curtime)
{
    s_physicsGravity = (vec3){0,-10,0};
    s_lastFrameTime = curtime;
    s_curPhysTime = curtime;

    s_defbody = (rigidbodyData_t){
        .mass = 1,
        .position = (vec3){-7,8,-8},
        .velocity = (vec3){0,0,0},
        .force = (vec3){0,0,0},
    };

    s_body = s_defbody;
}

void physics_simulate(rigidbodyData_t* body, float dt);

void physics_frame(float curtime)
{
    float dt = PHYSICS_FRAMETIME;
    while ( s_curPhysTime <= curtime )
    {
        physics_simulate(&s_body, dt);
        s_curPhysTime += dt;
    }

}

extern int castDown(float radius, vec3 origin, vec3* clip, vec3* norm);

void physics_simulate(rigidbodyData_t* body, float dt)
{
    // Check if we've fallen out of the world
    if(body->position.y < 0)
    {
        *body = s_defbody; 
        s_lastcamy = 1e9;
    }

    const vec3 g = s_physicsGravity;

    // Acceleration
    vec3 acc = g;

    
    
    // Are we in the ground?
    vec3 clip;
    vec3 norm;
    int hit = castDown(BALL_RADIUS, body->position, &clip, &norm);
    if(hit)
    {
        // Pop out of the ground
        body->position = addv3(body->position, clip);

    }

    if(hit)
    {
    
        float k, h;
        vec3 v;

        // Apply Normal Force
        k = -dotv3(g,g) / dotv3(norm, g);
        v = scalev3(norm, k);
        acc = addv3(acc, v);

        // Fight the urge to fall through the ground
        h = dotv3(norm, body->velocity) / magv3(norm);
        v = scalev3(norm, -h);
        body->velocity = addv3(body->velocity, v);

        // Friction force
        h = magv3(body->velocity);
        if(h > 0.01f)
        {
            v = scalev3(body->velocity, -0.08f * k / h);
            acc = addv3(acc, v);
        }
    }

    // Apply force
    acc = addv3(acc, scalev3(body->force, 1.0f / body->mass));


    //logInfo("%d - a %f %f %f", hit, acc.x, acc.y, acc.z);
    // Integrate
    body->velocity = addv3(body->velocity, scalev3(acc, dt));
    body->position = addv3(body->position, scalev3(body->velocity, dt));

    // Clear force
    body->force = (vec3){0,0,0};

    body->onground = hit;
}


#include <emscripten/emscripten.h>
#include "prop.h"
#include "camera.h"

extern camera_t s_cam;

EMSCRIPTEN_KEEPALIVE
void js_prop_sim(prop_t prop)
{
    //logInfo("%f %f %f", s_body.position.x, s_body.position.y, s_body.position.z);
    prop_setOrigin(prop, s_body.position);//addv3(s_body.position, (vec3){0.0, 0.25,0.0})); 
    float y = s_body.position.y + 4;
    if(y > s_lastcamy)
        y = s_lastcamy;
    else
        s_lastcamy = y;
    s_cam.origin = (vec3){s_body.position.x + 4.0f, y, s_body.position.z + 4.0f};
}

EMSCRIPTEN_KEEPALIVE
void js_applyForce(float x, float y, float z)
{
    s_body.force = addv3(s_body.force, (vec3){x,y,z}); 
}

EMSCRIPTEN_KEEPALIVE
int js_onGround()
{
    return s_body.onground;
}

