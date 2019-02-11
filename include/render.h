#ifndef RENDER_H
#define RENDER_H

#include "alg.h"
#include "mesh.h"
#include "state.h"

#define MAX_MESH 64
#define MAJOR 4
#define MINOR 0

struct Renderer
{
	Mesh *meshes[MAX_MESH];
	unsigned int objects[MAX_MESH];
	size_t mesh_count = 0;

	Color clear_color = Color { 0.3f, 0.2f, 0.1f };
	float fov = 60.0f;
	size_t color_loc;
	size_t model_loc;
	size_t proj_view_loc;

	void init(void*);
	void render(Mat4, State*);
	size_t add_mesh(Mesh*, bool compute_normals = true);
};

#endif
