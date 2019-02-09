#ifndef ENGINE_H
#define ENGINE_H

#include "game.h"
#include "mesh.h"
#include "instance.h"

#define MAX_MESH 64
#define MAX_ENTY 1024

struct Engine
{
	Mesh *meshes[MAX_MESH];
	unsigned int objects[MAX_MESH];
	size_t mesh_count;

	Instance enty_data[MAX_ENTY];
	size_t mesh_ids[MAX_ENTY];
	size_t enty_count;

	Color clear_color = Color { 0.3f, 0.2f, 0.1f };
	float fov = 60.0f;

	Engine();

	void init(Game&);
	size_t add_mesh(Mesh*, bool compute_normals = true);
	size_t add_entity(size_t);
	void update_entity(size_t, Instance);
};

#endif
