#include "camera.h"
#include "transform.h"
#include "shader.h"

void camera_apply(camera_t* cam)
{
    // View
    mat4x4 view, rot;
    identity4x4(&view);
    view.d = (vec4){-cam->origin.x, -cam->origin.y, -cam->origin.z, 1}; // Translate
    rotateXYZ4x4(&rot, -cam->rotation.x, -cam->rotation.y, -cam->rotation.z); // Rotate
    mul4x4(&view, &view, &rot);
    
    // Projection
    mat4x4 proj;
    perspective4x4(&proj, cam->fov, cam->near, cam->far, cam->aspect);

    // Set Uniforms
    shader_set(SHADER_UNIFORM_VIEW, &view);
    shader_set(SHADER_UNIFORM_PROJECTION, &proj);
    shader_set(SHADER_UNIFORM_CAMERAPOS, &cam->origin);
}
