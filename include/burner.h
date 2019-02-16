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



int Engine_init(void* custom_data = nullptr);
struct Engine {
};

#endif
