#VERTEX_SHADER
#version 300 es

// Vertex
in vec4 a_pos;
in vec4 a_norm;
in vec2 a_uv;

// Uniforms
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

// Outputs 
out vec2 v_uv;


void main(){
    // Make a new view mat that'll always follow the camera
    mat4 nview = u_view;
    nview[3] = vec4(0.0,0.0,0.0,1.0);

    v_uv=a_uv.xy;
    gl_Position = u_projection * nview * u_model * a_pos;
}

#END

#FRAGMENT_SHADER
#version 300 es

precision lowp float;

// Uniforms
uniform sampler2D tex;

// Inputs
in vec2 v_uv;

// Outputs
out vec4 o_fragColor;

void main() {
    o_fragColor = texture(tex,v_uv); 
}

#END