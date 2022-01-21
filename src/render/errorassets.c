#include "mesh.h"

// Baked in assets to pop up when something's broken

///////////////////
// ERROR TEXTURE //
///////////////////
unsigned int g_textureErrorPixels[] = 
{0xFF00FFFFu, 0xFFFF00FFu,
 0xFFFF00FFu, 0xFF00FFFFu};
unsigned int g_textureErrorWidth  = 2;
unsigned int g_textureErrorHeight = 2;

//////////////////
// ERROR SHADER //
//////////////////
const char g_shaderErrorVS[] =
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

const char g_shaderErrorFS[] =
    "precision lowp float;"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "void main(){"
    "    gl_FragColor=texture2D(tex,uv);"
    "}";


////////////////
// ERROR MESH //
////////////////
vtxprim_t g_meshErrorVBO[] =
{
	// Top face
	{{ 1, 1,-1}, { 0, 1, 0}, {0.5,0  }},
	{{-1, 1,-1}, { 0, 1, 0}, {0,  0  }},
	{{-1, 1, 1}, { 0, 1, 0}, {0,  0.5}},
	{{ 1, 1, 1}, { 0, 1, 0}, {0.5,0.5}},

	// Bottom face
	{{-1,-1,-1}, { 0,-1, 0}, {1,  0.5}},
	{{ 1,-1,-1}, { 0,-1, 0}, {0.5,0.5}},
	{{ 1,-1, 1}, { 0,-1, 0}, {0.5,1  }},
	{{-1,-1, 1}, { 0,-1, 0}, {1,  1  }},

	// Front face
	{{ 1, 1, 1}, { 0, 0, 1}, {0.5,0.5}},
	{{-1, 1, 1}, { 0, 0, 1}, {0,  0.5}},
	{{-1,-1, 1}, { 0, 0, 1}, {0,  1  }},
	{{ 1,-1, 1}, { 0, 0, 1}, {0.5,1  }},

	// Back face
	{{-1, 1,-1}, { 0, 0,-1}, {1,  0  }},
	{{ 1, 1,-1}, { 0, 0,-1}, {0.5,0  }},
	{{ 1,-1,-1}, { 0, 0,-1}, {0.5,0.5}},
	{{-1,-1,-1}, { 0, 0,-1}, {1,  0.5}},

	// Left face
	{{-1, 1, 1}, {-1, 0, 0}, {0.5,0.5}},
	{{-1, 1,-1}, {-1, 0, 0}, {0,  0.5}},
	{{-1,-1,-1}, {-1, 0, 0}, {0,  1  }},
	{{-1,-1, 1}, {-1, 0, 0}, {0.5,1  }},


	// Right face
	{{ 1, 1,-1}, { 1, 0, 0}, {1,  0  }},
	{{ 1, 1, 1}, { 1, 0, 0}, {0.5,0  }},
	{{ 1,-1, 1}, { 1, 0, 0}, {0.5,0.5}},
	{{ 1,-1,-1}, { 1, 0, 0}, {1,  0.5}},
};
unsigned short g_meshErrorIBO[] =
{
	// Top Face
	0, 1, 2,
	2, 3, 0,

	// Bottom Face
	4, 5, 6,
	6, 7, 4,

	// Front face
	8,   9, 10,
	10, 11,  8,

	// Back face
	12, 13, 14,
	14, 15, 12,
	
	// Left face
	16, 17, 18,
	18, 19, 16,

	// Right face
	20, 21, 22,
	22, 23, 20,
};
int g_meshErrorVN = sizeof(g_meshErrorVBO);
int g_meshErrorIN = sizeof(g_meshErrorIBO);
