#include <stdio.h>
#include "state.h"

size_t State::add_ent(size_t mesh_id)
{
	// Initialize default instance data
	this->ent_data[this->ent_count] = Instance {
		Color { 1, 1, 1 },
		Mat4::id()
	};

	// Set mesh reference
	this->mesh_ids[this->ent_count] = mesh_id;

	printf(
		"Added entity %lu with mesh %lu\n",
		this->ent_count,
		mesh_id
	);

	// Update entity ID counter
	return this->ent_count++;
}

void State::update_ent(size_t id, Instance data)
{
	this->ent_data[id] = data;
}
