#ifndef STATE_H
#define STATE_H

#include "instance.h"

#define MAX_ENTY 1024

struct State
{
	Instance ent_data[MAX_ENTY];
	size_t mesh_ids[MAX_ENTY];
	size_t ent_count = 0;

	ent add_ent(size_t);
	void update_ent(ent, Instance);
};

#endif
