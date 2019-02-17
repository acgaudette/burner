#ifndef BURNER_H
#define BURNER_H

#include "render.h"
#include "input.h"

typedef void (game_start)(State*, Renderer*, void**);
typedef game_start* (game_start_loader)();

typedef Mat4 (game_update)(State*, Renderer*, Input*, double, double, void*);
typedef game_update* (game_update_loader)();

typedef void (game_on_reload)(State*, Renderer*, void**);
typedef game_on_reload* (game_on_reload_loader)();

void start(State *state, Renderer *renderer, void **my_data);
Mat4 update(
    State *state,
    Renderer *renderer,
    Input *input,
    double time,
    double delta,
    void *my_data
);
void on_reload(State *state, Renderer *renderer, void **my_data);

// macro for start, update, ... procedure parameter lists?
//#define START_PROC(name_) name_##(State *state, Renderer *renderer, void **my_data)



int Engine_init(const char * const src_name, void* custom_data = nullptr);
struct Engine {
};

#endif
