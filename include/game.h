#ifndef GAME_H
#define GAME_H

#include "input.h"

struct Game
{
	virtual void start() = 0;
	virtual void update(Input, double, double) = 0;
};

#endif
