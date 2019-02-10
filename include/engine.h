#ifndef ENGINE_H
#define ENGINE_H

#include "mesh.h"
#include "instance.h"

#define MAX_MESH 64
#define MAX_ENTY 1024

void load_gl_functions();

struct Engine
{
	Mesh *meshes[MAX_MESH];
	unsigned int objects[MAX_MESH];
	size_t mesh_count;

	Instance ent_data[MAX_ENTY];
	size_t mesh_ids[MAX_ENTY];
	size_t ent_count;

	Color clear_color = Color { 0.3f, 0.2f, 0.1f };
	float fov = 60.0f;

	Engine();

	void init();
	size_t add_mesh(Mesh*, bool compute_normals = true);
	ent add_ent(size_t);
	void update_entity(ent, Instance);
};

#endif
