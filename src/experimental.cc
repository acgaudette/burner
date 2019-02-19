#include "experimental.h"

void init(SaveStates *states, size_t slot_count, size_t data_bsize)
{
    memset(states, 0, sizeof(SaveStates));
    states->data       = calloc_x(slot_count, data_bsize);
    states->slot_count = slot_count;
    states->data_bsize = data_bsize;
}

void deinit(SaveStates *states)
{
    free(states->data);
}
