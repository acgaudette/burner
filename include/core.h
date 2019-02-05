#ifndef CORE_H
#define CORE_H

#include "game.h"
#include "mesh.h"

#define MAX_MESH 64

struct Engine
{
	Mesh *meshes[MAX_MESH];
	unsigned int objects[MAX_MESH];
	size_t count;

	Engine();

	void init(Game&);
	void add_mesh(Mesh*);
};

#endif
