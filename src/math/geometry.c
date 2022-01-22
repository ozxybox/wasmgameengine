#include "geometry.h"
#include <math.h>

#define EPSILON 0.0001f

int rayPlaneTest(ray_t ray, vec3 normal, vec3 planePoint, vec3* poi, float* closestT)
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
    if (!(t <= *closestT))
        return 0;

    // 3D point of intersection
    vec3 p = addv3(ray.origin, scalev3(ray.dir, t));

    *closestT = t;
    *poi = p;
    return 1;
}

int rayTriangleTest(ray_t ray, tri_t tri, vec3 norm, vec3* poi, float* closestT)
{
    float pt = *closestT;
    vec3 p;
    int hit = rayPlaneTest(ray, norm, tri.a, &p, &pt);
    if (!hit)
        return 0;

	vec3 t = crossv3(subv3(tri.b,tri.a), norm);
	vec3 u = crossv3(subv3(tri.c,tri.b), norm);
	vec3 v = crossv3(subv3(tri.a,tri.c), norm);
	float k = dotv3(t,subv3(p,tri.a));
	float l = dotv3(u,subv3(p,tri.b));
	float m = dotv3(v,subv3(p,tri.c));
    if(k <= 0 && l <= 0 && m <= 0)
    {
        *closestT = pt;
        *poi = p;
        return 1;
    }
    return 0;
}

int sphereTriangleTest(float radius, vec3 origin, tri_t tri, vec3 norm, vec3* clip)
{
    // Did we hit and are we close enough to clip?
    float t = 9e9;
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
