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
out vec4 v_worldpos;
out vec3 v_normal;


void main(){
    v_uv=a_uv.xy;
    v_worldpos = u_model * a_pos;

    // Curve the world
    vec4 campos = u_view * v_worldpos;
    campos += vec4( 0.0f, (campos.z * campos.z) * -0.02f, 0.0f, 0.0f );
 
    gl_Position = u_projection * campos;
    v_normal = normalize(transpose(inverse(mat3(u_model))) * a_norm.xyz);
}

#END

#FRAGMENT_SHADER
#version 300 es

precision lowp float;

// Uniforms
uniform sampler2D tex;
uniform vec3 u_sundir;//lightpos;
uniform vec4 u_lightcolor;
uniform vec3 u_color;

// Inputs
in vec2 v_uv;
in vec4 v_worldpos;
in vec3 v_normal;

// Outputs
out vec4 o_fragColor;

void main() {
    vec3 lightDir = normalize(-u_sundir);//u_lightpos - v_worldpos.xyz);
    // Half Lambert
    float diff = pow(0.5 + 0.5 * dot(v_normal, lightDir), 2.0);
    vec3 diffuse = u_lightcolor.xyz * (diff + u_lightcolor.w);
    o_fragColor = texture(tex,v_uv) * vec4(diffuse, 1.0) * vec4(u_color.xyz, 1.0);
}

#END