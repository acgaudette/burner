#ifndef STATE_H
#define STATE_H

#include "alg.h"

#define MAX_ENTY 16384

typedef size_t ent;

struct Instance
{
	Color color;
	Mat4 model;
};

struct State
{
	Instance ent_data[MAX_ENTY];
	size_t mesh_ids[MAX_ENTY];
	size_t ent_count = 0;

	ent add_ent(size_t);
	void update_ent(ent, Instance);
};

#endif
