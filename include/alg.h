#ifndef ALG_H
#define ALG_H

struct Color
{
	float r;
	float g;
	float b;
};

struct Vec4
{
	float values[4];

	float dot(Vec4);
};

#endif
