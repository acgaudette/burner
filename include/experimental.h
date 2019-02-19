#ifndef BURNER_EXPERIMENTAL_H
#define BURNER_EXPERIMENTAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "burner.h"

inline void *malloc_x(size_t size)
{
    void *data = malloc(size);
    if (data == NULL) {
        fprintf(stderr, "Error malloc allocation failed");
        panic();
    }

    return data;
}

inline void *calloc_x(size_t count, size_t size)
{
    void *data = calloc(count, size);
    if (data == NULL) {
        fprintf(stderr, "Error calloc allocation failed");
        panic();
    }

    return data;
}

struct SaveStates {
    void* data;
    Input input_states[10];
    double time_states[10];
    double last_time_states[10];
    size_t slot_count;
    size_t data_bsize;
};

void init(SaveStates*, size_t, size_t);
void deinit(SaveStates*);

#endif


