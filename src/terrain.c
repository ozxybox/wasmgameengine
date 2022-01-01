#include "terrain.h"
#include "mesh.h"
#include <stdlib.h>
#include "log.h"

typedef struct 
{
    int width;
    int height;
    int* data;
} map_t;

static int s_terrainWidth  = 7;
static int s_terrainHeight = 7;
static int s_terrainData[] = {

    13, 2, 2, 2, 2, 2, 10,
     5, 1, 0, 0, 0, 1, 3,
     5, 0, 7, 4, 8, 0, 3,
     5, 0, 3, 14, 5, 0, 3,
     5, 0, 6, 2, 9, 0, 3,
     5, 1, 0, 0, 0, 1, 3,
    12, 4, 4, 4, 4, 4, 11,

};


typedef struct
{
    unsigned char height;
    unsigned char type;
} tile_t;

tile_t s_tilemap[8][8];


typedef struct {
    int topleft;
    int topright;
    int center;
    int bottomleft;
    int bottomright;
} tileData_t;


static tileData_t s_tileData[] = {

//  N
//W   E
//  S

{ // 0: Flat
    0,  0,
      0,
    0,  0,
},

{ // 1: Poke
    0,  0,
      2,
    0,  0,
},


{ // 2: Slope North
    2,  2,
      1,
    0,  0,
},

{ // 3: Slope East
    0,  2,
      1,
    0,  2,
},

{ // 4: Slope South
    0,  0,
      1,
    2,  2,
},

{ // 5: Slope West
    2,  0,
      1,
    2,  0,
},


{ // 6: Slope North-East
    0,  2,
      0,
    0,  0,
},

{ // 7: Slope South-East
    0,  0,
      0,
    0,  2,
},

{ // 8: Slope South-West
    0,  0,
      0,
    2,  0,
},

{ // 9: Slope North-West
    2,  0,
      0,
    0,  0,
},


{ // 10: Ledge North-East
    2,  2,
      2,
    0,  2,
},

{ // 11: Ledge South-East
    0,  2,
      2,
    2,  2,
},

{ // 12: Ledge South-West
    2,  0,
      2,
    2,  2,
},

{ // 13: Ledge North-West
    2,  2,
      2,
    2,  0,
},

// TEMP DELETE LATER
{ // 13: Ledge North-West
    2,  2,
      2,
    2,  2,
},

};

static unsigned short s_tileIBO[] = 
{
    0, 2, 1, // Up
    1, 2, 4, // Left
    4, 2, 3, // Bottom
    3, 2, 0, // Right
};



vertex_t s_tileVerts[] = {
    {
        .pos = {-1, 0, -1},
        .uv  = { 0, 0},
    },
    {
        .pos = { 1, 0, -1},
        .uv  = { 1, 0},
    },
    {
        .pos = { 0, 0,  0},
        .uv  = { 0.5f, 0.5f},
    },
    {
        .pos = {-1, 0,  1},
        .uv  = { 0, 1},
    },
    {
        .pos = { 1, 0,  1},
        .uv  = { 1, 1},
    },
};

#include "log.h"


typedef struct {
    tri_t tri;
    vec3 normal;
} coltri_t;

typedef struct {
    coltri_t tris[4];
    aabb_t bounds;
} coltile_t;



void tileDataToMesh(int id, vertex_t* vbo, coltri_t* colOut, float xoff, float zoff)
{
    tileData_t* dat = &s_tileData[id];
    int* weights = dat;

    for(int i = 0; i < 4; i++) // For each tile direction
    {
        /*
          B
         / \ 
        A   C

        M-TR, M-TL
        M-TL, TR-M
        */
        for(int k = 0; k < 3; k++) // For each weight
        {
            int t = s_tileIBO[i*3+k];
            vertex_t v = s_tileVerts[t];
            v.pos.y = weights[t] / 3.0f;
            v.pos.x += xoff;
            v.pos.z += zoff;
            vbo[i*3+k] = v;
            ((vec3*)&(colOut[i]).tri)[k] = v.pos;
        }
        vec3 norm = crossv3(subv3(vbo[i*3+1].pos, vbo[i*3+2].pos), subv3(vbo[i*3+1].pos, vbo[i*3+0].pos));
        norm = normalizev3(norm);
        colOut[i].normal = norm;

        for(int k = 0; k < 3; k++) // For each weight
        {
            vbo[i*3+k].norm = norm;
        }
    }

/*
    for(int i = 0; i < 12; i++) 
    {
        logInfo("%f %f %f", vbo[i].norm.x, vbo[i].norm.y, vbo[i].norm.z);
    }
*/

}

mesh_t loadTerrain(int width, int height, int* terrain, coltile_t** colmesh, int* triCountOut)
{
    int vertCount = 4 /*tris*/ * 3 /*verts*/ * width * height;
    vertex_t* vbo = malloc(sizeof(vertex_t) * vertCount);
    unsigned short* ibo = malloc(sizeof(unsigned short) * vertCount);
    *triCountOut = 4 * width * height;
    *colmesh = malloc(sizeof(coltile_t) * width * height);

    coltile_t* tilecol = *colmesh;
    vertex_t* tilevbo = vbo;
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            tileDataToMesh(terrain[y*width+x], tilevbo, tilecol->tris, x*2-width+1, y*2-height+1);
            tilevbo += 12;
            tilecol->bounds.mins = (vec3){x*2-width, 0, y*2-height};
            tilecol->bounds.maxs = (vec3){x*2-width+2, 0, y*2-height+2};
            tilecol++;
        }
    }

    // Fill with tris
    for(int i = 0; i < vertCount; i++)
        ibo[i] = i;

    mesh_t m = mesh_loadFromArray( vbo, vertCount, ibo, vertCount);

    free(vbo);
    free(ibo);
    
    logError("New tile %u", m);
    return m;
}

static coltile_t* s_colMesh;
static int s_colMeshCount;
mesh_t terrainTest()
{
    mesh_t m = loadTerrain(s_terrainWidth,s_terrainHeight,s_terrainData, &s_colMesh, &s_colMeshCount);
    logError("Terrain test %u", m);
    return m;
}

typedef struct {
    vec3 origin, dir;
}ray_t;

int rayPlaneTest(ray_t ray, vec3 normal, vec3 planePoint, vec3* poi)//float closestT)
{

    // Angle of approach on the face
    float approach = dotv3(ray.dir, normal);

    // If parallel to or not facing the plane, dump it
    // 
    // the ! < dumps malformed triangles with NANs!
    // does not serve the same purpose as >=
    if (!(approach < 0.0f))
        return 0;

    // All points on the plane have the same dot, defined as d
    float d = dotv3(planePoint, normal);

    // Parametric point of intersection with the plane of the tri and the ray
    float t = (d - dotv3(ray.origin, normal)) / approach;

    // (p0 + tD) * n = d
    // tD*n = d - p0*d

    // Ignore backside and NANs
    if (!(t >= 0.0f))
        return 0;

    // If something else was closer, skip our current and NANs
    //if (!(t <= closestT))
    //    return 0;

    // 3D point of intersection
    vec3 p = addv3(ray.origin, scalev3(ray.dir, t));

    *poi = p;
    return 1;
}

int rayTriangleTest(ray_t ray, coltri_t coltri, vec3* poi, vec3* onorm)//float closestT)
{
    tri_t tri = coltri.tri;

    vec3 norm = coltri.normal;
    
    vec3 p;
    int hit = rayPlaneTest(ray, norm, tri.a, &p);//closestT);
    if (!hit)
        return 0;// {false};

	vec3 t = crossv3(subv3(tri.a,tri.b), norm);
	vec3 u = crossv3(subv3(tri.b,tri.c), norm);
	vec3 v = crossv3(subv3(tri.c,tri.a), norm);
	float k = dotv3(t,subv3(p,tri.a));
	float l = dotv3(u,subv3(p,tri.b));
	float m = dotv3(v,subv3(p,tri.c));
    if(k >= 0 && l >= 0 && m >= 0)
    {
        *poi = p;
        *onorm = norm;
        return 1;
    }
    return 0;
}

int castDown(vec3 point, vec3* poi, vec3* norm)
{
    ray_t ray;
    ray.dir = (vec3){0,-1,0};
    ray.origin = point;

    for(int i = 0; i < s_colMeshCount; i++)
    {
        coltile_t* tile = s_colMesh + i;

        if(point.x < tile->bounds.mins.x || point.x > tile->bounds.maxs.x
        || point.z < tile->bounds.mins.z || point.z > tile->bounds.maxs.z)
            continue;

        
        int hit = 0;        
        //vec3 p = {0,0,0};
        for(int k = 0; k < 4; k++)
            if(rayTriangleTest(ray, tile->tris[k], poi, norm))
                return 1;
        //if(hit)
        //    return p.y;
        

    }

    return 0;
}

/*
EMSCRIPTEN_KEEPALIVE
mesh_t js_tileDataToMesh(int id)
{
    return 
float castDown(vec3 point)(id);
}*/
#include <emscripten/emscripten.h>

EMSCRIPTEN_KEEPALIVE
mesh_t js_terrainTest()
{

    return terrainTest();
}

/*
EMSCRIPTEN_KEEPALIVE
float js_castDown(float x, float y, float z)
{
    return castDown((vec3){x,y,z});
}
*/




