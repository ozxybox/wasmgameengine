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

static int s_terrainWidth  = 8;
static int s_terrainHeight = 9;
static int s_terrainData[9*8][2] = {

    { 1, 9}, {18, 9}, {15, 9}, {-1, 0}, {-1, 0}, { 3, 6}, { 3, 6}, {-1, 0},
    { 1, 9}, { 6, 9}, {19, 9}, { 5, 7}, { 5, 7}, { 3, 5}, { 3, 5}, {-1, 0},
    { 1, 9}, {17, 9}, { 5, 9}, {20, 8}, {21, 8}, { 3, 4}, { 3, 4}, {-1, 0},
    {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, { 5, 4}, { 5, 4}, {-1, 0},
    { 4, 1}, {19, 2}, { 3, 2}, { 3, 2}, { 3, 2}, { 3, 2}, { 3, 2}, {15, 2},
    { 4, 1}, { 0, 2}, { 0, 2}, { 0, 2}, { 0, 2}, { 0, 2}, { 0, 2}, { 4, 2},
    { 4, 1}, { 0, 2}, {23, 1}, { 0, 2}, { 0, 2}, { 0, 2}, { 0, 2}, { 4, 2},
    { 4, 1}, { 0, 2}, { 0, 2}, { 0, 2}, { 0, 2}, { 0, 2}, { 0, 2}, {19, 2},
    {19, 1}, { 3, 1}, { 3, 1}, { 3, 1}, { 3, 1}, { 3, 1}, { 3, 1}, { 3, 1},

/*
    { 0, 0}, { 0, 0}, {0, 0}, {0, 0}, { 0, 4}, {5, 3}, {5, 2}, { 0, 2}, { 3, 2},
    { 0, 0}, { 0, 0}, {0, 0}, {0, 0}, { 0, 0}, {0, 0}, {0, 0}, {14, 1}, { 3, 2},
    { 0, 0}, { 0, 0}, {0, 0}, {0, 0}, { 0, 0}, {0, 0}, {0, 0}, {15, 1}, { 3, 2},
    { 5, 1}, { 5, 0}, {0, 0}, {0, 0}, { 0, 0}, {0, 0}, {0, 0}, { 0, 0}, { 0, 0},
    { 5, 1}, { 5, 0}, {0, 0}, {0, 0}, { 0, 0}, {0, 0}, {0, 0}, { 0, 0}, { 0, 0},
    { 5, 1}, { 5, 0}, {0, 0}, {0, 0}, { 0, 0}, {0, 0}, {0, 0}, { 0, 0}, { 0, 0},
    { 5, 1}, { 5, 0}, {0, 0}, {0, 0}, { 0, 0}, {0, 0}, {0, 0}, { 0, 0}, { 0, 0},
    { 5, 1}, {12, 0}, {4, 0}, {4, 0}, { 4, 0}, {4, 0}, {4, 0}, { 0, 0}, { 0, 0},
    {12, 1}, { 4, 1}, {4, 1}, {4, 1}, { 4, 1}, {4, 1}, {4, 1}, { 4, 1}, { 0, 0},
*/


};
/*
  13, 2, 2, 2, 2, 2, 10,
     5, 1, 0, 0, 0, 1, 3,
     5, 0, 7, 4, 8, 0, 3,
     5, 0, 3, 14, 5, 0, 3,
     5, 0, 6, 2, 9, 0, 3,
     5, 1, 0, 0, 0, 1, 3,
    12, 4, 4, 4, 4, 4, 11,

*/

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


{ // 1: filled
    2,  2,
      2,
    2,  2,
},

{ // 2: Poke
    0,  0,
      1,
    0,  0,
},


{ // 3: Slope North
    2,  2,
      1,
    0,  0,
},

{ // 4: Slope East
    0,  2,
      1,
    0,  2,
},

{ // 5: Slope South
    0,  0,
      1,
    2,  2,
},

{ // 6: Slope West
    2,  0,
      1,
    2,  0,
},


{ // 7: Slope North-East
    0,  2,
      0,
    0,  0,
},

{ // 8: Slope South-East
    0,  0,
      0,
    0,  2,
},

{ // 9: Slope South-West
    0,  0,
      0,
    2,  0,
},

{ // 10: Slope North-West
    2,  0,
      0,
    0,  0,
},


{ // 11: Ledge North-East
    2,  2,
      2,
    0,  2,
},

{ // 12: Ledge South-East
    0,  2,
      2,
    2,  2,
},

{ // 13: Ledge South-West
    2,  0,
      2,
    2,  2,
},

{ // 14: Ledge North-West
    2,  2,
      2,
    2,  0,
},


{ // 15: Slope North-East
    2,  2,
      1,
    0,  2,
},

{ // 16: Slope South-East
    0,  2,
      1,
    2,  2,
},

{ // 17: Slope South-West
    2,  0,
      1,
    2,  2,
},

{ // 18: Slope North-West
    2,  2,
      1,
    2,  0,
},


{ // 19: Slope North-East
    0,  2,
      1,
    0,  0,
},

{ // 20: Slope South-East
    0,  0,
      1,
    0,  2,
},

{ // 21: Slope South-West
    0,  0,
      1,
    2,  0,
},

{ // 22: Slope North-West
    2,  0,
      1,
    0,  0,
},


{ // 23: Poke
    2,  2,
      0,
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



vtxprim_t s_tileVerts[] = {
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



void tileDataToMesh(int id, vtxprim_t* vbo, coltri_t* colOut, float xoff, float zoff, float yoff)
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
            vtxprim_t v = s_tileVerts[t];
            v.pos.y = (weights[t] + yoff) / 3.0f;
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

    vtxbuf_t vbuf;
    vtxbuf_alloc(VTXPRIM_FORMAT, vertCount, &vbuf);

    idxbuf_t ibuf;
    idxbuf_alloc(vertCount, &ibuf);

    *triCountOut = 4 * width * height;
    *colmesh = malloc(sizeof(coltile_t) * width * height);

    coltile_t* tilecol = *colmesh;
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            int* p = &terrain[(y*width+x)*2];
            int tile = p[0];
            int yoff = p[1] * 2;

            vtxprim_t* tilevbo = vtxbuf_gorge(&vbuf, 12);
            if(tile >= 0) 
                tileDataToMesh(tile, tilevbo, tilecol->tris, x*2-width+1, y*2-height+1, yoff);

            tilecol->bounds.mins = (vec3){x*2-width, 0, y*2-height};
            tilecol->bounds.maxs = (vec3){x*2-width+2, 0, y*2-height+2};
            tilecol++;
        }
    }

    // Fill with tris
    unsigned short* ibo = idxbuf_gorge(&ibuf, vertCount);
    for(int i = 0; i < vertCount; i++)
        ibo[i] = i;

    mesh_t m = mesh_loadFromArray(&vbuf, &ibuf);

    vtxbuf_free(&vbuf);
    idxbuf_free(&ibuf);
    
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
/*
    |p-o|=r

    |p-o|<=r
    |p-o|<=r
    k.n

    // (p0 + nR) . n = d
    // tR.n = d - p0.n
    // t = (d - p0.n)/R.n
*/
int rayPlaneTest(ray_t ray, vec3 normal, vec3 planePoint, vec3* poi, float* ot)//float closestT)
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

    // (p0 + tR) . n = d
    // tR.n = d - p0.n
    // t = (d - p0.n)/R.n

    // Ignore backside and NANs
    if (!(t >= 0.0f))
        return 0;

    // If something else was closer, skip our current and NANs
    //if (!(t <= closestT))
    //    return 0;

    // 3D point of intersection
    vec3 p = addv3(ray.origin, scalev3(ray.dir, t));

    *ot = t;
    *poi = p;
    return 1;
}

int rayTriangleTest(ray_t ray, coltri_t coltri, vec3* poi, float* ot)//float closestT)
{
    tri_t tri = coltri.tri;
    vec3 norm = coltri.normal;
    
    float pt;
    vec3 p;
    int hit = rayPlaneTest(ray, norm, tri.a, &p, &pt);//closestT);
    if (!hit)
        return 0;// {false};

    // NOTE: With terrain tris being in a X on a square, could we just rotate them to a +?
    // Then it'd be a simple > or < check? Is in Tile && Is in Tri?

	vec3 t = crossv3(subv3(tri.b,tri.a), norm);
	vec3 u = crossv3(subv3(tri.c,tri.b), norm);
	vec3 v = crossv3(subv3(tri.a,tri.c), norm);
	float k = dotv3(t,subv3(p,tri.a));
	float l = dotv3(u,subv3(p,tri.b));
	float m = dotv3(v,subv3(p,tri.c));
    if(k <= 0 && l <= 0 && m <= 0)
    {
        *ot = pt;
        *poi = p;
        return 1;
    }
    return 0;
}

#define EPSILON 0.0001f
#include <math.h>
int spherePlaneTest(float radius, vec3 origin, coltri_t coltri, vec3* clip)
{
    tri_t tri = coltri.tri;
    vec3 norm = coltri.normal;
    
    // Did we hit and are we close enough to clip?
    float t;
    vec3 p;
    int hit = rayPlaneTest((ray_t){origin, scalev3(norm, -1.0f)}, norm, tri.a, &p, &t);
    if (!hit || t > radius)
        return 0;

    // Edges
    vec3 e[][2] = {
        {tri.a, subv3(tri.b,tri.a)},
        {tri.b, subv3(tri.c,tri.b)},
        {tri.c, subv3(tri.a,tri.c)},
    };

    // Is our point in our tri?
    int intri = 1;
    for(int i = 0; i < 3 && intri; i++)
    	intri &= dotv3(crossv3(e[i][1], norm),subv3(p,e[i][0])) <= 0;
    if(intri)
    {
        // Inside the tri
        *clip = scalev3(norm, radius - t); 
        //logInfo("AA %f - %f %f %f", 1.0f - t / radius, clip->x, clip->y, clip->z);
        return 1;
    }

    // Did we intersect with an edge?
    float r2 = radius*radius;
    for(int i = 0; i < 3; i++)
    {
        float m, b, g, d0, d1, d;
        vec3 u, f, c;

        // Quadratic equation to find the intersection of the edge with the sphere
        m = magv3(e[i][1]);
        u = scalev3(e[i][1],1.0f/m);
        f = subv3(origin,e[i][0]);
        b = dotv3(u,f); 
        g = b*b-dotv3(f,f)+r2;

        if(g <= 0)
            continue;
        g = sqrt(g);
        
        // 2 POIs
        d0 = b-g;
        d1 = b+g;

        // Are we on the edge at all?
        // d0 < b < d1
        if(d1 < 0 || d0 > m)
            continue; 

        // Put it all together
        d = b + (d0>=0?g:0) - (d1 <= m?g:0);
        
        // Clip
        c = subv3(origin, addv3(e[i][0], scalev3(u, d)));
        float q = 1.0f - magv3(c) / radius;

        // Skip exact perfect intersections
        if(q <= EPSILON)
            continue;

        // Return the clip
        c = scalev3(c, q);
        *clip = c;
        //logInfo("BB %f - %f %f %f", q, clip->x, clip->y, clip->z);
        return 1;
    }

    // Nothing
    return 0;
}



int castDown(float radius, vec3 origin, vec3* clip, vec3* norm)
{
    // Convert from world position to tile position on the board
    float tx = (origin.x+s_terrainWidth)/2.0f;
    float ty = (origin.z+s_terrainHeight)/2.0f;

    vec3 clipdisp = (vec3){0,0,0};
    vec3 clipnorm = (vec3){0,0,0};
    int hit = 0;

    // Everything within 2x2 tiles
    for(int tileY = floor(ty-0.5f); tileY < ceil(ty+0.5); tileY++)
    {
        for(int tileX = floor(tx-0.5f); tileX < ceil(tx+0.5); tileX++)
        {
            // Out of bounds?
            if(tileX >= s_terrainWidth || tileX < 0
            || tileY >= s_terrainHeight || tileY < 0)
                continue;

            coltile_t* tile = s_colMesh + (int)tileY * s_terrainWidth + (int)tileX;

            // Test for all triangles for this tile
            for(int k = 0; k < 4; k++)
            {
                // Sphere collision
                vec3 c;
                if(spherePlaneTest(radius, origin, tile->tris[k], &c))
                {
                    hit++;
                    clipdisp = addv3(clipdisp, c);
                    clipnorm = addv3(clipnorm, tile->tris[k].normal);

                    //return 1;
                    break;
                }
            }

        }
    }

    // Output
    if(hit)
    {
        *norm = normalizev3(clipnorm);
        *clip = clipdisp;
        return 1;
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




