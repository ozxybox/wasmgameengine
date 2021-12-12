#include "texture.h"

#include <stdlib.h>
#include <string.h>
#include <ogl.h>
#include "log.h"


typedef struct textureData_t
{
    GLuint id;
    
    unsigned int width;
    unsigned int height;
    
    struct textureData_t* next;

} textureData_t;

static textureData_t s_textures[MAX_TEXTURES];
static unsigned int s_textureCount = 0;

static texture_t s_pErrorTexture = 0;

static textureData_t* newTextureData()
{
    textureData_t* dat = s_textures + s_textureCount;
    s_textureCount++;
 
    return dat;
}

static texture_t textureDataToIdx(textureData_t* data)
{
    size_t off = data - s_textures;

    if(off >= MAX_TEXTURES)
        return TEXTURE_INVALID_INDEX;
    return off;
}

static GLuint createGLTexture()
{
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_NEAREST);//GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST);//GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  return texture;
}


void textureSystemInit()
{
   unsigned int errorTexturePix[] = 
   {0xFF00FFFFu, 0xFFFF00FFu,
    0xFFFF00FFu, 0xFF00FFFFu};
   s_pErrorTexture = loadTexture(&errorTexturePix[0], 2, 2);

}

void textureSystemShutdown()
{

}

 
texture_t errorTexture()
{
    return s_pErrorTexture;
}


// Empty slot for placing a texture later
texture_t emptyTexture()
{
    textureData_t* tex = newTextureData();
    tex->width = 0;
    tex->height = 0;
    tex->id = GL_INVALID_INDEX;

    return textureDataToIdx(tex);
}

// Loads data into an empty texture
void loadIntoTexture(texture_t texture, unsigned char* pixels, unsigned int width, unsigned int height)
{
    if(s_textureCount == 0 || texture >= s_textureCount) return;

    textureData_t* tex = &s_textures[texture];
    tex->width = width;
    tex->height = height;
    tex->id = createGLTexture();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
}


// Texture creating
texture_t loadTexture(unsigned char* pixels, unsigned int width, unsigned int height)
{
    texture_t tex = textureDataToIdx(newTextureData());
    loadIntoTexture(tex, pixels, width, height);
    return tex;
}


void bindTexture(texture_t tex)
{
    if(s_textureCount == 0) return;
    textureData_t* dat = &s_textures[tex];
    if(dat->id == GL_INVALID_INDEX)
        glBindTexture(GL_TEXTURE_2D, s_textures[s_pErrorTexture].id);
    else
        glBindTexture(GL_TEXTURE_2D, dat->id);
}


void setTextureDimensions(texture_t texture, int* w, int* h)
{
    if(s_textureCount == 0 || texture >= s_textureCount) return;

    textureData_t* tex = &s_textures[texture];

    if(w)
        tex->width = *w;
    if(h)
        tex->height = *h;
} 
void getTextureDimensions(texture_t texture, int* w, int* h)
{
    if(s_textureCount == 0 || texture >= s_textureCount) return;

    textureData_t* tex = &s_textures[texture];

    if(w)
        *w = tex->width;
    if(h)
        *h = tex->height;
}
