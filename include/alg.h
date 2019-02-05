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

struct Mat4
{
	// Column-major
	float values[16];

	Mat4();
	Mat4(float[16]);

	static Mat4 id();
	static Mat4 perspective(float, float, float, float);

	Mat4 operator*(const Mat4&);
};

#endif
