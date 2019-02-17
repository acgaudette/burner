#include "simple_common.h"

Mesh *mesh;
ent obj;

// create custom function to hoist the custom data into global scope for convenience
void init_global(CustomData *data)
{
	mesh = &data->mesh;
	obj = data->obj;
}

void start(State *state, Renderer *renderer, void **my_data)
{
	CustomData* data = *(CustomData**)my_data;

	float alt = sqrt(3) * 0.5f;
	float apo = alt / 3;
	float rad = alt - apo;

	float proj = sqrt(alt * alt - apo * apo);
	float low = proj * 0.25;
	float up = 3 * low;

	float pos[] = {
		 0.0, -low,  rad,
		 0.5, -low, -apo,
		-0.5, -low, -apo,

		 0.0, -low,  rad,
		 0.5, -low, -apo,
		-0.5, -low, -apo,

		 0.0, up, 0.0 // Peak
	};

	unsigned int ind[] = {
		0, 2, 1,
		3, 4, 6,
		4, 5, 6,
		5, 3, 6
	};
	memcpy(data->indices, ind, sizeof(ind));

	data->mesh = Mesh(&data->vertices[0], 7, pos, &data->indices[0], 12);

	size_t tetra = renderer->add_mesh(&data->mesh);
	data->obj = state->add_ent(tetra);

	init_global(data);
}

Mat4 update(
	State *state,
	Renderer *renderer,
	Input *input,
	double time,
	double delta,
	void *my_data
) {
	//CustomData* data = (CustomData*)my_data;

	Instance instance;
	instance.color = Color { 1, 0.5f, 0.3f };
	instance.model = Mat4::translation(0, -0.25f, 1.5f)
		* Mat4::rotation_y(time);
	state->update_ent(obj, instance);

	return Mat4::id();
}

void on_reload(State *state, Renderer *renderer, void **my_data)
{
	init_global(*(CustomData**)my_data);
}
