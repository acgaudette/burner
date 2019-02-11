#ifndef MESH_H
#define MESH_H

#define VERT_STRIDE 6

#include <stddef.h>
#include "alg.h"

struct Vertex
{
	Vec3 position;
	Vec3 normal;
};

struct Mesh
{
	Vertex *vertices;
	size_t vertex_count;
	unsigned int *indices;
	size_t index_count;

	Mesh();
	Mesh(Vertex*, size_t, float*, unsigned int*, size_t);
	Mesh(Vertex*, size_t, float*, float*, unsigned int*, size_t);
};

#endif
