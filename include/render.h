#ifndef RENDER_H
#define RENDER_H

#include "state.h"
#include "mesh.h"

#define WIDTH  1280
#define HEIGHT 720
#define MAJOR 4
#define MINOR 0
#define MAX_MESH 1024

void panic();

struct Renderer
{
	Color clear_color = Color { 0.3f, 0.2f, 0.1f };
	float fov = 60.0f;

	Mesh *meshes[MAX_MESH];
	unsigned int objects[MAX_MESH];
	size_t mesh_count = 0;

	size_t color_loc;
	size_t model_loc;
	size_t proj_view_loc;

	void init(void*);
	void render(Mat4, State*);
	size_t add_mesh(Mesh*, bool compute_normals = true);
};

#endif
