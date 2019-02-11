#ifndef GAME_H
#define GAME_H

#include "alg.h"
#include "state.h"
#include "render.h"
#include "input.h"

typedef void (game_start)(State*, Renderer*);
typedef game_start* (game_start_loader)();

typedef Mat4 (game_update)(State*, Renderer*, Input, double, double);
typedef game_update* (game_update_loader)();

#endif
