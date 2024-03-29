#include "assetdecode.h"
#include "log.h"
#include "objimport.h"
#include "stringutils.h"
#include <lodepng/lodepng.h>
#include <stdlib.h>

void texture_loadIntoFromPNG(texture_t texture, const unsigned char* png, unsigned int size)
{
    unsigned char* image = 0;
    unsigned int width, height;

    unsigned int error = lodepng_decode32(&image, &width, &height, png, size);
    if(error)
        logError("Failed to decode png! Error %u: %s\n", error, lodepng_error_text(error));
    else
        texture_loadInto(texture, image, width, height);

    if(image) free(image);
}
texture_t texture_loadFromPNG(const unsigned char* png, unsigned int size)
{
    unsigned char* image = 0;
    unsigned int width, height;

    texture_t texture = texture_error();
    unsigned int error = lodepng_decode32(&image, &width, &height, png, size);
    if(error)
        logError("Failed to decode png! Error %u: %s\n", error, lodepng_error_text(error));
    else
        texture = texture_load(image, width, height);

    free(image);

    return texture;
}


meshBufferData_t loadMeshDataFromObj(const unsigned char* obj, unsigned int size)
{
    objmodel_t model;
    decodeOBJ(obj, size, &model);

    //logInfo("MESH COUNT %d", model.meshCount);

    meshBufferData_t* meshes = malloc(sizeof(meshBufferData_t) * model.meshCount);

    for(int meshidx = 0; meshidx < model.meshCount; meshidx++ )
    {

        objmesh_t* mesh = &model.meshes[meshidx];

        unsigned int elements = 0;
        for(int i = 0; i < mesh->faceCount; i++)
            elements += mesh->faces[i].vertexCount - 2;
        elements *= 3;

        
        idxbuf_t indices;
        idxbuf_alloc(&indices, elements);
        unsigned short* idx = indices.ibo;
        unsigned int curele = 0;
        for(int i = 0; i < mesh->faceCount; i++)
        {
            // Triangulate
            objface_t* face = mesh->faces + i;
            for(int k = 1; k < face->vertexCount - 1; k++)
            {
                *idx++ = curele;
                *idx++ = curele + k;
                *idx++ = curele + k + 1;
            }
            curele += face->vertexCount;
        }
        indices.used = curele;
        
        vtxbuf_t vertices;
        vtxformat_t fmt = VTX_POSITION | VTX_NORMAL | VTX_TEXCOORD;
        vtxbuf_alloc(&vertices, fmt, curele);
        for(int i = 0; i < mesh->faceCount; i++)
        {
            objface_t* face = mesh->faces + i;
            for(int k = 0; k < face->vertexCount; k++)
            {
                vertex_t* v = vtxbuf_eat(&vertices); 
                *vtx_pos (fmt, v) = model.verts[face->vertices[k].vertex];
                *vtx_norm(fmt, v) = model.norms[face->vertices[k].normal];
                *vtx_uv  (fmt, v) = model.uvs  [face->vertices[k].uv    ];
            }
        }
        //logInfo("MODEL %d %d", curele, elements);

        meshes[meshidx] = (meshBufferData_t){vertices, indices};
    }

    freeOBJ(&model);
    
    return *meshes;
}

#define BEGIN_VS "#VERTEX_SHADER"
#define BEGIN_FS "#FRAGMENT_SHADER"
#define END_SHADER "#END"

void parseShaderFile(char* shader, shaderParseData_t* out)
{
    // We dumped some preprocess text in the shaders so that we don't need to use separate files
    // Separate it now

    char* vsStart = 0;
    int vsLen = 0;
    char* fsStart = 0;
    int fsLen = 0;

    // 1: vs, 2: fs, 0: none 
    int inShader = 0;
    char* shaderStart = shader;

    for(char* c = shader; *c;)
    {

        if(!inShader)
        {

            if(!strncmp(BEGIN_VS, c, sizeof(BEGIN_VS) - 1))
            {
                inShader = 1;
                // Skip until EOL
                for (; *c && !IsEndLine(*c); c++);
                // Skip until not EOL
                for (; *c && IsEndLine(*c); c++);
                shaderStart = c;
                continue;
            }
            else if(!strncmp(BEGIN_FS, c, sizeof(BEGIN_FS) - 1))
            {
                inShader = 2;
                // Skip until EOL
                for (; *c && !IsEndLine(*c); c++);
                // Skip until not EOL
                for (; *c && IsEndLine(*c); c++);
                shaderStart = c;
                continue;
            }
        }
        else if(!strncmp(END_SHADER, c, sizeof(END_SHADER) - 1))
        {
            unsigned int len = c - shaderStart;
            if(inShader == 1)
            {
                vsStart = shaderStart;
                vsLen = len;
            } 
            else if (inShader == 2)
            {
                fsStart = shaderStart;
                fsLen = len;
            }
            inShader = 0;
        }
        // Skip until EOL
        for (; *c && !IsEndLine(*c); c++);
        // Skip until not EOL
        for (; *c && IsEndLine(*c); c++);
    }

    char* str = malloc(vsLen + fsLen + 2);
    memcpy(str, vsStart, vsLen);
    str[vsLen] = 0;
    memcpy(str+vsLen+1, fsStart, fsLen);
    str[vsLen+1+fsLen] = 0;

    out->buf = str;
    out->vs = str;
    out->vsSize = vsLen;
    out->fs = str+vsLen+1;
    out->fsSize = fsLen;
    
}