#ifndef GAME_H
#define GAME_H

#include "alg.h"
#include "input.h"

struct Game
{
	virtual void start() = 0;
	virtual Mat4 update(Input, double, double) = 0;
};

#endif
