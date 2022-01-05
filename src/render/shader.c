#include "shader.h"
#include "linalg.h"
#include "log.h"
#include "map.h"
#include "linkedlist.h"
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
    vec3   color;
} uniformData_t;
static uniformLayoutInfo_t s_uniformLayout[] = {
    UNIFORM(MAT4,   model)
    UNIFORM(MAT4,   view)
    UNIFORM(MAT4,   projection)
    UNIFORM(FLOAT3, camerapos)
    UNIFORM(FLOAT3, lightpos)
    UNIFORM(FLOAT4, lightcolor)
    UNIFORM(FLOAT3, sundir)
    UNIFORM(FLOAT3, color)
};
#undef UNIFORM

#define UNIFORM_COUNT (sizeof(s_uniformLayout)/sizeof(uniformLayoutInfo_t))

typedef struct {
    shader_t id;

    GLuint program;
    
    GLuint uniforms[UNIFORM_COUNT];
} shaderInfo_t;

static uniformData_t s_uniformData;

static map_t s_shaderMap;
static linklist_t s_shaderList;
static shader_t s_shaderId;

static shaderInfo_t* s_currentShader = 0;
static shaderInfo_t* s_errorShader = 0;


static void checkError(GLuint shader)
{
    int infoLogLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
        char vertexError[1024];
        glGetShaderInfoLog(shader, infoLogLength, NULL, &vertexError[0]);
        logError("[shader] Error:\n%s", vertexError);
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

static void createProgram(shaderInfo_t* info, GLuint vertexShader, GLuint fragmentShader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glBindAttribLocation(program, 0, "a_pos");
    glBindAttribLocation(program, 1, "a_norm");
    glBindAttribLocation(program, 2, "a_uv");
    glLinkProgram(program);
    //checkError(program);
    
    info->program = program;

    //colorPos = glGetUniformLocation(program, "color");
    
    // Get all uniforms for this shader
    for(int i = 0; i < sizeof(s_uniformLayout) / sizeof(uniformLayoutInfo_t); i++)
        info->uniforms[i] = glGetUniformLocation(program, s_uniformLayout[i].name);
}


static shaderInfo_t* newShaderInfo()
{
    // Get a new key id
    unsigned int k = s_shaderId;
    s_shaderId++;

    // Make space for a shader on the list
    shaderInfo_t m;
    linkitem_t* li = ll_push(&s_shaderList, &m);
    void* dat = ll_access(li);

    // Place it in the map
    map_set(&s_shaderMap, k, dat);

    // Setup the data
    memset(dat, GL_INVALID_INDEX, sizeof(shaderInfo_t));
    shaderInfo_t* info = (shaderInfo_t*)dat;
    info->id = k;

    // Pass back the shader
    return info;
}

static int shaderInfoFromId(shader_t shader, shaderInfo_t** out)
{
    // Pull in our data from our id
    shaderInfo_t* dat;
    if(map_get(&s_shaderMap, shader, (void**)&dat))
    {
        if(out) *out = dat;
        return 1;
    }
    else
    {
        // FIXME: Too annyoing //logError("[shader] (%u) does not exist!", shader);
        
        if(out) *out = s_errorShader;
        return 0;
    }
}

static void loadIntoShader(shaderInfo_t* info, const char* vs, const char* fs)
{
    GLuint vertexShader   = compileShader(GL_VERTEX_SHADER, vs);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fs);
    
    createProgram(info, vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

shader_t shader_empty()
{
    shaderInfo_t* info = newShaderInfo();
    return info->id;
}

void shader_delete(shader_t shader)
{
    shaderInfo_t* info;
    if(map_get(&s_shaderMap, shader, (void**)&info))  
    {
        if(info->program != GL_INVALID_INDEX)
            glDeleteProgram(info->program);
        map_remove(&s_shaderMap, shader);
        ll_remove(&s_shaderMap, ll_item(info));
    }
}

void shader_loadInto(shader_t empty, const char* vs, const char* fs)
{
    shaderInfo_t* info;
    if(!map_get(&s_shaderMap, empty, (void**)&info))
        return;

    loadIntoShader(info, vs, fs);
}
shader_t shader_create(const char* vs, const char* fs)
{
    shaderInfo_t* info = newShaderInfo();
    loadIntoShader(info, vs, fs);

    return info->id;
}


// Applies the uniform data to the shader
void applyUniform(int idx)
{
    if(!s_currentShader) return;

    GLuint uniform = s_currentShader->uniforms[idx];
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
void shader_set(int uniform, void* data)
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

void shader_bind(shader_t shader)
{    
    shaderInfo_t* info = 0;
    if(!map_get(&s_shaderMap, shader, &info))
    {
        // FIXME: Too annyoing //logError("[shader] (%u) does not exist!", shader);
        info = s_errorShader;
    }

    if(info->program == GL_INVALID_INDEX) 
    {
        if(info == s_errorShader) return;
        info = s_errorShader;
    }
    s_currentShader = info;

    glUseProgram(info->program);
    setAllUniforms();

}


extern const char g_shaderErrorVS[];
extern const char g_shaderErrorFS[];

void shader_systemInit()
{
    logInfo("[shader] System Init");

    s_shaderId = 0;
    s_shaderMap = map_create();
    s_shaderList = ll_create(sizeof(shaderInfo_t));

    // Load a default shader
    s_errorShader = newShaderInfo();
    loadIntoShader(s_errorShader, g_shaderErrorVS, g_shaderErrorFS);
    s_currentShader = 0;
}
void shader_systemShutdown()
{
     logInfo("[shader] System Shutdown");

    // Clear out all GL data
    for(linkitem_t* li = s_shaderList.first; li; li = li->next)
        glDeleteProgram(((shaderInfo_t*)ll_access(li))->program);

    // Clear out the lists
    ll_clear(&s_shaderList);
    map_clear(&s_shaderMap);
}
