#include "objimport.h"
#include "linkedlist.h"
#include "stringutils.h"
#include <stdlib.h>
#include <string.h>


static char* PullString(char* c, objstring_t* out)
{
	char* start = c;
	for (; *c && !IsEndLine(*c); c++);

	unsigned int len = c - start;
	char* str = malloc(len + 1u);
	strncpy(str, start, len);
	str[len] = 0;
	
	*out = (objstring_t){ str, len };

	return c;
}

static char* PullTexture(char* c, objtexture_t* out)
{
	return PullString(c, &out->name);
	return c;
}

int LoadMtlString(char* content, mtlfile_t* mtl)
{
	if (!content || !mtl)
		return 0;


	linklist_t mats = initLinkedList(sizeof(objmaterial_t));

	objmaterial_t mat;

	for (char* c = content; *c; c++)
	{
		if (IsWhitespace(*c))
			continue;
			
		if (strncmp("newmtl ", c, 7) == 0)
		{
			c += 7;

			if (mat.name.str)
			{
				pushLinkedItem(&mats, &mat);

			}
			memset(&mat, 0, sizeof(objmaterial_t));

			c = PullString(c, &mat.name);


		}
		else if (strncmp("Ka ", c, 3) == 0)
		{
			c += 3;
			
			c = StringToVec(c, &mat.ambientColor);
		}
		else if (strncmp("Kd ", c, 3) == 0)
		{
			c += 3;

			c = StringToVec(c, &mat.diffuseColor);
		}
		else if (strncmp("Ks ", c, 3) == 0)
		{
			c += 3;

			c = StringToVec(c, &mat.specularColor);
		}
		else if (strncmp("Kf ", c, 3) == 0)
		{
			c += 3;

			c = StringToVec(c, &mat.transmissionFilterColor);
		}
		else if (strncmp("Kf ", c, 3) == 0)
		{
			c += 3;
		
			mat.specularExponent = strtod(c, &c);
		}
		else if (strncmp("d ", c, 2) == 0)
		{
			c += 2;

			mat.opacity = strtod(c, &c);
		}
		else if (strncmp("Tr ", c, 3) == 0)
		{
			c += 3;

			mat.opacity = 1.0 - strtod(c, &c);
		}
		else if (strncmp("Ni ", c, 3) == 0)
		{
			c += 3;

			mat.opticalDensity = strtod(c, &c);
		}
		else if (strncmp("map_Kd ", c, 7) == 0)
		{
			c += 7;

			c = PullTexture(c, &mat.diffuseTx);
		}
		else if (strncmp("map_Ks ", c, 7) == 0)
		{
			c += 7;

			c = PullTexture(c, &mat.specularTx);
		}
		else if (strncmp("map_Ns ", c, 7) == 0)
		{
			c += 7;

			//c = PullTexture(c, mat.);
		}
		else
		{
			// Unsupported, read to eol
			for (; *c && !IsEndLine(*c); c++);
		}
	}

}

void decodeOBJ(const char* buf, unsigned int size, objmodel_t* out)
{
	if (!buf || !size || !out)
		return;


	// Yuck! Replace this with a linked block stack or something later?
	linklist_t meshes = initLinkedList(sizeof(objmesh_t));
	linklist_t faces = initLinkedList(sizeof(objface_t));
	linklist_t norms = initLinkedList(sizeof(vec3));
	linklist_t verts = initLinkedList(sizeof(vec3));
	linklist_t texcoords = initLinkedList(sizeof(vec2));

	objmesh_t curmesh = {{0,0}, 0, 0};

	for (char* c = (char*)buf; *c; c++)
	{
		if (IsWhitespace(*c))
			continue;
		/*
		if (strncmp("g ", c, 2) == 0)
		{
			c += 2;

			// New Group
			if(currentgroup.)

			pushLinkedItem(&norms, &n);
		}
		else */
		if (strncmp("vn ", c, 3) == 0)
		{
			c += 3;

			// Normal
			vec3 n = { 0,0,0 };
			n.x = strtod(c, &c);
			n.y = strtod(c, &c);
			n.z = strtod(c, &c);

			pushLinkedItem(&norms, &n);
		}
		else if (strncmp("vt ", c, 3) == 0)
		{
			c += 3;

			// Texture Coord
			vec2 t = { 0,0 };
			t.x = strtod(c, &c);
			t.y = 1.0f - strtod(c, &c);

			pushLinkedItem(&texcoords, &t);

		}
		else if (strncmp("v ", c, 2) == 0)
		{
			c += 2;

			// Position
			vec3 v = { 0,0,0 };
			v.x = strtod(c, &c);
			v.y = strtod(c, &c);
			v.z = strtod(c, &c);

			pushLinkedItem(&verts, &v);
		}
		else if (*c == 'f')
		{
			// Face
			// Parse the face til eol
			c += 2;

			linklist_t faceverts = initLinkedList(sizeof(objvert_t));

			while (c - buf < size)
			{
				for (; c - buf < size && IsWhitespace(*c) && !IsEndLine(*c); c++);
				if (IsEndLine(*c))
					break;

				unsigned int vert = 0, text = 0, norm = 0;

				vert = strtoul(c, &c, 10);
				if (*c == '/')
					text = strtoul(++c, &c, 10);
				if (*c == '/')
					norm = strtoul(++c, &c, 10);

				objvert_t fv = {.vertex = vert - 1, .uv = text - 1, .normal = norm - 1}; 
				
				pushLinkedItem(&faceverts, &fv);
			}
			if (faceverts.count)
			{
				objface_t face;
				face.vertexCount = faceverts.count;
				face.vertices = (objvert_t*)malloc(sizeof(objvert_t) * faceverts.count);
				
				copyLinkedListInto(face.vertices, &faceverts, sizeof(objvert_t) * faceverts.count);

				pushLinkedItem(&faces, &face);
			}
			clearLinkedList(&faceverts);

		}
		else if (strncmp("mtllib ", c, 7) == 0)
		{
			c += 7;
			

			// Copy it into a temp buf so we can null terminate it
			char* start = c;
			for (; *c && !IsEndLine(*c); c++);
			
			unsigned int len = c - start;
			char* matname = malloc(len + 1u);
			strncpy(matname, start, len);
			matname[len] = 0;
			
			unsigned int fc = faces.count - (unsigned int)curmesh.faces;
			if(fc > 0)
			{
				curmesh.faceCount = fc;
				pushLinkedItem(&meshes, &curmesh);
			}
			curmesh = (objmesh_t){{matname, len}, faces.count - curmesh.faceCount, faces.count};

		}
		else
		{
			// Unsupported, read to eol
			for (; c - buf < size && !IsEndLine(*c); c++);
		}
	}

	unsigned int meshfc = faces.count - (unsigned int)curmesh.faces;
	if(meshfc > 0)
	{
		curmesh.faceCount = meshfc;
		pushLinkedItem(&meshes, &curmesh);
	}

	//out->faceCount = faces.count;
	out->facepool = malloc(sizeof(objface_t) * faces.count);
	copyLinkedListInto(out->facepool, &faces, faces.count * sizeof(objface_t));

	out->normCount = norms.count;
	out->norms = malloc(sizeof(vec3) *norms.count);
	copyLinkedListInto(out->norms, &norms, norms.count * sizeof(vec3));

	out->vertCount = verts.count;
	out->verts = malloc(sizeof(vec3) * verts.count);
	copyLinkedListInto(out->verts, &verts, verts.count * sizeof(vec3));

	out->uvCount = texcoords.count;
	out->uvs = malloc(sizeof(vec2) * texcoords.count);
	copyLinkedListInto(out->uvs, &texcoords, texcoords.count * sizeof(vec3));

	out->meshCount = meshes.count;
	out->meshes = malloc(sizeof(objmesh_t) * meshes.count);
	copyLinkedListInto(out->meshes, &meshes, meshes.count * sizeof(objmesh_t));

	// Fill in the mesh data with real pointers
	for(int i = 0; i < out->meshCount; i++)
	{
		out->meshes[i].faces = out->facepool + (unsigned int)out->meshes[i].faces;
	}

	clearLinkedList(&faces);
	clearLinkedList(&norms);
	clearLinkedList(&verts);
	clearLinkedList(&texcoords);
}


void freeOBJ(objmodel_t* obj)
{
	for(int i = 0; i < obj->meshCount; i++)
		free(obj->meshes[i].material.str);
	free(obj->facepool);
	free(obj->verts);
	free(obj->norms);
	free(obj->uvs);
}