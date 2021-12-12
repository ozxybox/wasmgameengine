#include "transform.h"


transform_t transform()
{
    return (transform_t){{0,0,0}, {0,0,0}, {0,0,0}};
}

void transformMatrix(transform_t* transform, mat4x4* matrix)
{
    mat4x4 mat, rot;
    identity4x4(&mat);

    // Scale    
    vec4 scale = {transform->scale.x, transform->scale.y, transform->scale.z, 1};
    scale4x4v4(&mat, &mat, &scale);

    rotateXYZ4x4(&rot, transform->rotation.x, transform->rotation.y, transform->rotation.z);
    mul4x4(&mat, &mat, &rot);
    
    // Translate
    mat.d = (vec4){transform->origin.x, transform->origin.y, transform->origin.z, 1};

    *matrix = mat;
}

/*
logInfo("[%f %f %f %f\n %f %f %f %f\n %f %f %f %f\n %f %f %f %f]", 
model.a.x, model.b.x, model.c.x, model.d.x, 
model.a.y, model.b.y, model.c.y, model.d.y,
model.a.z, model.b.z, model.c.z, model.d.z,
model.a.w, model.b.w, model.c.w, model.d.w);
*/