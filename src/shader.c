#include "shader.h"
#include "linalg.h"
#include "log.h"
#include <ogl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

enum UNIFORM_TYPE
{
    UNIFORM_FLOAT1,
    UNIFORM_FLOAT2,
    UNIFORM_FLOAT3,
    UNIFORM_FLOAT4,

    UNIFORM_INT1,
    UNIFORM_INT2,
    UNIFORM_INT3,
    UNIFORM_INT4,

    UNIFORM_MAT2,
    UNIFORM_MAT3,
    UNIFORM_MAT4,
};
const unsigned int s_uniformTypeSize[] = 
{
    sizeof(float)*1,
    sizeof(float)*2,
    sizeof(float)*3,
    sizeof(float)*4,

    sizeof(int)*1,
    sizeof(int)*2,
    sizeof(int)*3,
    sizeof(int)*4,

    sizeof(float)*2*2,
    sizeof(float)*3*3,
    sizeof(float)*4*4,
};

typedef struct 
{
    int type;
    unsigned int offset;
    const char* name;
} uniformLayoutInfo_t;
#define UNIFORM(type, name) {UNIFORM_ ## type, offsetof(uniformData_t, name), "u_" #name},


// This should match the enum
typedef struct {
    mat4x4 model;
    mat4x4 view;
    mat4x4 projection;
    vec3   camerapos;
    vec3   lightpos;
    vec4   lightcolor;
    vec3   sundir;
} uniformData_t;
static uniformLayoutInfo_t s_uniformLayout[] = {
    UNIFORM(MAT4,   model)
    UNIFORM(MAT4,   view)
    UNIFORM(MAT4,   projection)
    UNIFORM(FLOAT3, camerapos)
    UNIFORM(FLOAT3, lightpos)
    UNIFORM(FLOAT4, lightcolor)
    UNIFORM(FLOAT3, sundir)
};
#undef UNIFORM

#define UNIFORM_COUNT (sizeof(s_uniformLayout)/sizeof(uniformLayoutInfo_t))

static uniformData_t s_uniformData;

typedef struct {
    GLuint program;
    
    GLuint uniforms[UNIFORM_COUNT];
} shaderInfo_t;

#define MAX_SHADERS 16
static shaderInfo_t s_shaders[MAX_SHADERS]; 
static unsigned int s_shaderCount = 0;
static shader_t s_currentShader = SHADER_INVALID_INDEX;


void checkError(GLuint shader)
{
    int InfoLogLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        char vertexError[1024];
        glGetShaderInfoLog(shader, InfoLogLength, NULL, &vertexError[0]);
        logError(vertexError);
    }
}

static GLuint compileShader(GLenum shaderType, const char *src)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    checkError(shader);
    return shader;
}

static shaderInfo_t createProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glBindAttribLocation(program, 0, "a_pos");
    glBindAttribLocation(program, 1, "a_norm");
    glBindAttribLocation(program, 2, "a_uv");
    glLinkProgram(program);
    //checkError(program);
    
    shaderInfo_t info;
    info.program = program;

    //colorPos = glGetUniformLocation(program, "color");
    
    // Get all uniforms for this shader
    for(int i = 0; i < sizeof(s_uniformLayout) / sizeof(uniformLayoutInfo_t); i++)
        info.uniforms[i] = glGetUniformLocation(program, s_uniformLayout[i].name);


    return info;
}

shaderInfo_t* newShaderInfo()
{
    shaderInfo_t* info = &s_shaders[s_shaderCount];
    s_shaderCount++;
    return info;
}

shader_t emptyShader()
{
    shaderInfo_t* info = newShaderInfo();
    info->program = GL_INVALID_INDEX;
    return info - &s_shaders[0];
}
void loadIntoShader(shader_t empty, const char* vs, const char* fs)
{
    logMsg(vs);
    logMsg(fs);
    GLuint vertexShader   = compileShader(GL_VERTEX_SHADER, vs);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fs);
    
    shaderInfo_t* info = &s_shaders[empty];
    *info = createProgram(vertexShader, fragmentShader);
}
shader_t createShader(const char* vs, const char* fs)
{
    GLuint vertexShader   = compileShader(GL_VERTEX_SHADER, vs);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fs);
    
    shaderInfo_t* info = newShaderInfo();
    *info = createProgram(vertexShader, fragmentShader);

    return info - &s_shaders[0];
}


// Applies the uniform data to the shader
void applyUniform(int idx)
{
    if(s_shaderCount == 0) return;
    if(s_currentShader >= s_shaderCount) return;

    shaderInfo_t* shader = &s_shaders[s_currentShader];
    GLuint uniform = ((GLuint*)&shader->uniforms)[idx];
    if(uniform == GL_INVALID_INDEX) return;
    
    void* mem = (char*)&s_uniformData + s_uniformLayout[idx].offset;
    switch (s_uniformLayout[idx].type)
    {
    case UNIFORM_FLOAT1: glUniform1fv(uniform, 1, mem); break;
    case UNIFORM_FLOAT2: glUniform2fv(uniform, 1, mem); break;
    case UNIFORM_FLOAT3: glUniform3fv(uniform, 1, mem); break;
    case UNIFORM_FLOAT4: glUniform4fv(uniform, 1, mem); break;
    
    case UNIFORM_INT1: glUniform1iv(uniform, 1, mem); break;
    case UNIFORM_INT2: glUniform2iv(uniform, 1, mem); break;
    case UNIFORM_INT3: glUniform3iv(uniform, 1, mem); break;
    case UNIFORM_INT4: glUniform4iv(uniform, 1, mem); break;

    case UNIFORM_MAT2: glUniformMatrix2fv(uniform, 1, 0, mem); break;
    case UNIFORM_MAT3: glUniformMatrix3fv(uniform, 1, 0, mem); break;
    case UNIFORM_MAT4: glUniformMatrix4fv(uniform, 1, 0, mem); break;
    
    default:
        break;
    }
}
void setUniform(int uniform, void* data)
{
    unsigned int size = s_uniformTypeSize[s_uniformLayout[uniform].type];
    void* mem = (char*)&s_uniformData + s_uniformLayout[uniform].offset;

    memcpy(mem, data, size);
    applyUniform(uniform);
}

void setAllUniforms()
{
    for(int i = 0; i < sizeof(s_uniformLayout) / sizeof(uniformLayoutInfo_t); i++)
        applyUniform(i);
}

void bindShader(shader_t shader)
{    
    if(s_shaderCount == 0) return;
    if(s_currentShader >= s_shaderCount)
        shader = 0;

    shaderInfo_t* info = &s_shaders[shader];
    if(info->program == GL_INVALID_INDEX) 
    {
        if(shader != 0) bindShader(0);
        return;
    }
    s_currentShader = shader;

    glUseProgram(info->program);
    setAllUniforms();

}


void shaderSystemInit()
{
    // Load a default shader into position 0
    static const char vs[] =
    "attribute vec4 a_pos;"
    "attribute vec4 a_norm;"
    "attribute vec2 a_uv;"
    "varying vec2 uv;"
    "uniform mat4 u_model;"
    "uniform mat4 u_view;"
    "uniform mat4 u_projection;"
    "void main(){"
    "    uv=a_uv.xy;"
    "    gl_Position = u_projection * u_view * u_model * a_pos;"
    "}";

    static const char fs[] =
    "precision lowp float;"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "void main(){"
    "    gl_FragColor=texture2D(tex,uv);"
    "}";

    shader_t s = createShader(vs, fs);

    logInfo("Shader System Init %d", s);
}