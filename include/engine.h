#ifndef ENGINE_H
#define ENGINE_H

#include "game.h"
#include "mesh.h"
#include "instance.h"

#define MAX_MESH 64
#define MAX_ENTY 1024

struct Engine
{
	int width;
	int height;

	Mesh *meshes[MAX_MESH];
	unsigned int objects[MAX_MESH];
	size_t mesh_count;

	Instance enty_data[MAX_ENTY];
	size_t mesh_ids[MAX_ENTY];
	size_t enty_count;

	Mat4 view;

	Engine();

	void init(Game&);
	void add_mesh(Mesh*);
};

#endif
