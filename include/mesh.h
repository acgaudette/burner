#ifndef MESH_H
#define MESH_H

#include "render.h"

struct Mesh
{
	Vertex *vertices;
	size_t vertex_count;

	unsigned int *indices;
	size_t index_count;
};

#endif
