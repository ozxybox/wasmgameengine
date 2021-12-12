#include "primitives.h"
   
static vertex_t s_cubeVbo[] =
{
	// Top face
	{{ 1, 1,-1}, { 0, 1, 0}, {1,0}},
	{{-1, 1,-1}, { 0, 1, 0}, {0,0}},
	{{-1, 1, 1}, { 0, 1, 0}, {0,1}},
	{{ 1, 1, 1}, { 0, 1, 0}, {1,1}},

	// Bottom face
	{{-1,-1,-1}, { 0,-1, 0}, {1,0}},
	{{ 1,-1,-1}, { 0,-1, 0}, {0,0}},
	{{ 1,-1, 1}, { 0,-1, 0}, {0,1}},
	{{-1,-1, 1}, { 0,-1, 0}, {1,1}},

	// Front face
	{{ 1, 1, 1}, { 0, 0, 1}, {1,0}},
	{{-1, 1, 1}, { 0, 0, 1}, {0,0}},
	{{-1,-1, 1}, { 0, 0, 1}, {0,1}},
	{{ 1,-1, 1}, { 0, 0, 1}, {1,1}},

	// Back face
	{{-1, 1,-1}, { 0, 0,-1}, {1,0}},
	{{ 1, 1,-1}, { 0, 0,-1}, {0,0}},
	{{ 1,-1,-1}, { 0, 0,-1}, {0,1}},
	{{-1,-1,-1}, { 0, 0,-1}, {1,1}},

	// Left face
	{{-1, 1, 1}, {-1, 0, 0}, {1,0}},
	{{-1, 1,-1}, {-1, 0, 0}, {0,0}},
	{{-1,-1,-1}, {-1, 0, 0}, {0,1}},
	{{-1,-1, 1}, {-1, 0, 0}, {1,1}},


	// Right face
	{{ 1, 1,-1}, { 1, 0, 0}, {1,0}},
	{{ 1, 1, 1}, { 1, 0, 0}, {0,0}},
	{{ 1,-1, 1}, { 1, 0, 0}, {0,1}},
	{{ 1,-1,-1}, { 1, 0, 0}, {1,1}},
};

static unsigned short s_cubeIbo[] =
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

static vertex_t s_skyVbo[] =
{
	// Top face
	{{ 1, 1,-1}, { 0,-1, 0}, {1.0f/4.0f,     0.0f}},
	{{-1, 1,-1}, { 0,-1, 0}, {2.0f/4.0f,     0.0f}},
	{{-1, 1, 1}, { 0,-1, 0}, {2.0f/4.0f,1.0f/3.0f}},
	{{ 1, 1, 1}, { 0,-1, 0}, {1.0f/4.0f,1.0f/3.0f}},

	// Bottom face
	{{-1,-1,-1}, { 0, 1, 0}, {2.0f/4.0f,     1.0f}},
	{{ 1,-1,-1}, { 0, 1, 0}, {1.0f/4.0f,     1.0f}},
	{{ 1,-1, 1}, { 0, 1, 0}, {1.0f/4.0f,2.0f/3.0f}},
	{{-1,-1, 1}, { 0, 1, 0}, {2.0f/4.0f,2.0f/3.0f}},

	// Front face
	{{ 1, 1, 1}, { 0, 0,-1}, {1.0f/4.0f,1.0f/3.0f}},
	{{-1, 1, 1}, { 0, 0,-1}, {2.0f/4.0f,1.0f/3.0f}},
	{{-1,-1, 1}, { 0, 0,-1}, {2.0f/4.0f,2.0f/3.0f}},
	{{ 1,-1, 1}, { 0, 0,-1}, {1.0f/4.0f,2.0f/3.0f}},

	// Back face
	{{-1, 1,-1}, { 0, 0, 1}, {3.0f/4.0f,1.0f/3.0f}},
	{{ 1, 1,-1}, { 0, 0, 1}, {4.0f/4.0f,1.0f/3.0f}},
	{{ 1,-1,-1}, { 0, 0, 1}, {4.0f/4.0f,2.0f/3.0f}},
	{{-1,-1,-1}, { 0, 0, 1}, {3.0f/4.0f,2.0f/3.0f}},

	// Left face
	{{-1, 1, 1}, { 1, 0, 0}, {2.0f/4.0f,1.0f/3.0f}},
	{{-1, 1,-1}, { 1, 0, 0}, {3.0f/4.0f,1.0f/3.0f}},
	{{-1,-1,-1}, { 1, 0, 0}, {3.0f/4.0f,2.0f/3.0f}},
	{{-1,-1, 1}, { 1, 0, 0}, {2.0f/4.0f,2.0f/3.0f}},


	// Right face
	{{ 1, 1,-1}, {-1, 0, 0}, {     0.0f,1.0f/3.0f}},
	{{ 1, 1, 1}, {-1, 0, 0}, {1.0f/4.0f,1.0f/3.0f}},
	{{ 1,-1, 1}, {-1, 0, 0}, {1.0f/4.0f,2.0f/3.0f}},
	{{ 1,-1,-1}, {-1, 0, 0}, {     0.0f,2.0f/3.0f}},
};

static unsigned short s_skyIbo[] =
{
	// Top Face
	2, 1, 0,
	0, 3, 2,

	// Bottom Face
	6, 5, 4,
	4, 7, 6,

	// Front face
	10,  9,  8,
	 8, 11, 10,

	// Back face
	14, 13, 12,
	12, 15, 14,
	
	// Left face
	18, 17, 16,
	16, 19, 18,

	// Right face
	22, 21, 20,
	20, 23, 22,
};

static mesh_t s_primCubeMesh = MESH_INVALID_INDEX;
static mesh_t s_primSkyMesh = MESH_INVALID_INDEX;

void primitiveSystemInit()
{
    s_primCubeMesh = loadMeshFromArray(s_cubeVbo, sizeof(s_cubeVbo) / sizeof(vertex_t), s_cubeIbo, sizeof(s_cubeIbo) / sizeof(unsigned short));
    s_primSkyMesh = loadMeshFromArray(s_skyVbo, sizeof(s_skyVbo) / sizeof(vertex_t), s_skyIbo, sizeof(s_skyIbo) / sizeof(unsigned short));

}

mesh_t cubeMesh()
{
    return s_primCubeMesh;
}

mesh_t skyMesh()
{
    return s_primSkyMesh;
}

mesh_t errorMesh()
{
    return s_primCubeMesh;
}


/*
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    const float pos[] = 
    { -1, 0, 0,  0, 1,
       1, 0, 0,  1, 1,
      -1, 1, 0,  0, 0,
       1, 1, 0,  1, 0
        };
*/