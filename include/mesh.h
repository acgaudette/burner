#ifndef MESH_H
#define MESH_H

#include "alg.h"

struct Mesh
{
	float *vertices;
	size_t vertex_count;

	unsigned int *indices;
	size_t index_count;

	Color color;
};

#endif
