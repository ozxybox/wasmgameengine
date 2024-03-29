#include "camera.h"
#include "transform.h"
#include "shader.h"

void camera_apply(camera_t* cam)
{
    // View
    mat4x4 view, rot;
    identity4x4(&view);
    view.d = (vec4){-cam->origin.x, -cam->origin.y, -cam->origin.z, 1}; // Translate
    
    mat4x4 mat;
    rotateZAxis4x4(&mat, -cam->rotation.z); // Roll
    rotateYAxis4x4(&rot, -cam->rotation.y); // Yaw
    mul4x4(&mat, &rot, &mat);
    rotateXAxis4x4(&rot, -cam->rotation.x); // Pitch
    mul4x4(&rot, &rot, &mat);
    
    //rotateXYZ4x4(&rot, , -cam->rotation.y, -cam->rotation.z); // Rotate
    mul4x4(&view, &rot, &view);
    
    // Projection
    mat4x4 proj;
    perspective4x4(&proj, cam->fov, cam->near, cam->far, cam->aspect);

    // Set Uniforms
    shader_set(SHADER_UNIFORM_VIEW, &view);
    shader_set(SHADER_UNIFORM_PROJECTION, &proj);
    shader_set(SHADER_UNIFORM_CAMERAPOS, &cam->origin);
}
