#include "texture.h"
#include "log.h"
#include "linkedlist.h"
#include "map.h"

#include <stdlib.h>
#include <string.h>
#include <ogl.h>


typedef struct textureData_t
{
    unsigned int id;
    GLuint glid;
    
    unsigned int width;
    unsigned int height;    
} textureData_t;

static linklist_t s_textureList;
static map_t s_textureMap;
static unsigned int s_textureId;

static textureData_t* s_pErrorTexture = 0;

static textureData_t* newTextureData()
{
    // Get a new key id
    unsigned int k = s_textureId;
    s_textureId++;

    // Set up the data
    textureData_t m = {
        .id = k,

        .glid = GL_INVALID_INDEX,
        .width = 0,
        .height = 0,
    };

    // Make space for it on the list
    linkitem_t* li = ll_push(&s_textureList, &m);
    void* dat = ll_access(li);

    // Place it in the map
    map_set(&s_textureMap, k, dat);

    // Pass back the texture
    return (textureData_t*)dat;
}

static int textureDataFromId(texture_t texture, textureData_t** out)
{
    // Pull in our data from our id
    textureData_t* dat;
    if(map_get(&s_textureMap, texture, (void**)&dat))
    {
        if(out) *out = dat;
        return 1;
    }
    else
    {
        // FIXME: Too annyoing //logError("[texture] (%u) does not exist!", texture);
        
        if(out) *out = s_pErrorTexture;
        return 0;
    }
}

static GLuint createGLTexture()
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP_TO_EDGE);
    return texture;
}

static void deleteTextureGL(textureData_t* tex)
{
    if(tex->glid != GL_INVALID_INDEX)
        glDeleteTextures(1, &tex->glid);
}

static void loadIntoTexture(textureData_t* tex, void* pixels, unsigned int width, unsigned int height)
{
    tex->width = width;
    tex->height = height;
    tex->glid = createGLTexture();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}
extern unsigned int g_textureErrorPixels[];
extern unsigned int g_textureErrorWidth;
extern unsigned int g_textureErrorHeight;


void texture_systemInit()
{
    logInfo("[texture] System Init");

    s_textureId = 0;
    s_textureList = ll_create(sizeof(textureData_t));
    s_textureMap = map_create();

    // Create error texture
    s_pErrorTexture = newTextureData();
    loadIntoTexture(s_pErrorTexture, &g_textureErrorPixels[0], g_textureErrorWidth, g_textureErrorHeight);
}

void texture_systemShutdown()
{
    logInfo("[texture] System Shutdown");

    // Clear out all GL data
    for(linkitem_t* li = s_textureList.first; li; li = li->next)
        deleteTextureGL(ll_access(li));

    // Clear out the lists
    ll_clear(&s_textureList);
    map_clear(&s_textureMap);
}

void texture_systemReload()
{
    texture_systemShutdown();
    texture_systemInit();
}

texture_t texture_error()
{
    return s_pErrorTexture->id;
}

// Empty slot for placing a texture later
texture_t texture_empty()
{
    texture_t k = newTextureData()->id;
    return k;
}

void texture_delete(texture_t texture)
{
    textureData_t* tex;
    if(map_get(&s_textureMap, texture, (void**)&tex))  
    {
        deleteTextureGL(tex);
        map_remove(&s_textureMap, texture);
        ll_remove(&s_textureMap, ll_item(tex));
    }
}

// Loads data into an empty texture
void texture_loadInto(texture_t texture, unsigned char* pixels, unsigned int width, unsigned int height)
{
    textureData_t* tex;
    if(map_get(&s_textureMap, texture, (void**)&tex))  
        loadIntoTexture(tex, pixels, width, height);
    //else
        // FIXME: Too annyoing //logError("[texture] (%u) does not exist!", texture);
}


// Texture creating
texture_t texture_load(unsigned char* pixels, unsigned int width, unsigned int height)
{
    textureData_t* tex = newTextureData();
    loadIntoTexture(tex, pixels, width, height);
    return tex->id;
}


void texture_bind(texture_t texture)
{
    textureData_t* tex;
    textureDataFromId(texture, &tex);

    if(tex->glid == GL_INVALID_INDEX)
        glBindTexture(GL_TEXTURE_2D, s_pErrorTexture->glid);
    else
        glBindTexture(GL_TEXTURE_2D, tex->glid);
}


void texture_setDimensions(texture_t texture, unsigned int* w, unsigned int* h)
{
    textureData_t* tex;
    if(textureDataFromId(texture, &tex))  
    {
        if(w)
            tex->width = *w;
        if(h)
            tex->height = *h;
    }
    //else
        // FIXME: Too annyoing //logError("[texture] (%u) does not exist!", texture);
} 

void texture_getDimensions(texture_t texture, unsigned int* w, unsigned int* h)
{
    textureData_t* tex;
    if(textureDataFromId(texture, &tex))  
    {
        if(w)
            *w = tex->width;
        if(h)
            *h = tex->height;
    }
    //else
        // FIXME: Too annyoing //logError("[texture] (%u) does not exist!", texture);
}
