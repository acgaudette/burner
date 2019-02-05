#ifndef GAME_H
#define GAME_H

struct Game
{
	virtual void start() = 0;
	virtual void update(double, double) = 0;
};

#endif
