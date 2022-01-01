#include "physics.h"
#include "log.h"

// How long is one frame of simulations?
#define PHYSICS_FRAMETIME (1.0f / 60.0f) // 60 ups


typedef struct rigidbodyData_t
{
    float mass;
    vec3 position;
    vec3 velocity;
    vec3 force;
    //vec3 acceleration;
} rigidbodyData_t;

static float s_lastFrameTime;
static float s_curPhysTime;

static vec3 s_physicsGravity;

rigidbodyData_t s_body;

void physics_systemInit(float curtime)
{
    s_physicsGravity = (vec3){0,-10,0};
    s_lastFrameTime = curtime;
    s_curPhysTime = curtime;

    s_body = (rigidbodyData_t){
        .mass = 1,
        .position = (vec3){0,10,0},
        .velocity = (vec3){0,0,0},
        .force = (vec3){0,0,0},
    };
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

extern int castDown(vec3 point, vec3* poi, vec3* onorm);

void physics_simulate(rigidbodyData_t* body, float dt)
{
    vec3 poi;
    vec3 norm;
    int hit = castDown((vec3){body->position.x, 100, body->position.z}, &poi, &norm);

    if(!hit)
    {
        body->position = (vec3){0, 10, 0};
    }

    // Acceleration
    vec3 acc = s_physicsGravity;

    if(hit && body->position.y <= poi.y)
    {
        // Force normal
    
        // In ground, apply normal force
        float k = -magv3(acc) / dotv3(norm, acc);
        vec3 fn = scalev3(norm, k);
        acc = addv3(acc, fn);



        // Friction force
        vec3 vn = normalizev3(body->velocity);
        vec3 fr = scalev3(vn, -0.5f * k);// * magv3(crossv3(norm, vn)));
        acc = addv3(acc, fr);

        // Pop out of the ground
        body->position = poi;
        body->velocity.y = 0;//(vec3){0,0,0};
    }


    // Apply force
    acc = addv3(acc, scalev3(body->force, 1.0f / body->mass));


    // Integrate
    body->velocity = addv3(body->velocity, scalev3(acc, dt));
    body->position = addv3(body->position, scalev3(body->velocity, dt));

    // Clear force
    body->force = (vec3){0,0,0};
}


#include <emscripten/emscripten.h>
#include "prop.h"

EMSCRIPTEN_KEEPALIVE
void js_prop_sim(prop_t prop)
{
    //logInfo("%f %f %f", s_body.position.x, s_body.position.y, s_body.position.z);
    prop_setOrigin(prop, addv3(s_body.position, (vec3){0.0, 0.25,0.0})); 
}

EMSCRIPTEN_KEEPALIVE
void js_applyForce(float x, float y, float z)
{
    s_body.force = addv3(s_body.force, (vec3){x,y,z}); 
}


