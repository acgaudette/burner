#ifndef GAME_H
#define GAME_H

#include "engine.h"
#include "alg.h"
#include "input.h"

typedef void (game_start)(Engine*);
typedef game_start* (game_start_loader)();

typedef Mat4 (game_update)(Engine*, Input, double, double);
typedef game_update* (game_update_loader)();

#endif
