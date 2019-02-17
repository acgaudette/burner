#ifndef ALG_H
#define ALG_H

struct Color
{
	float r;
	float g;
	float b;

	Color operator*(const float&);
	Color operator*(const Color&);
	Color operator+(const Color&);
};

struct Vec2
{
	float x;
	float y;

	float mag_squared();
	float mag();
	Vec2 norm();
	Vec2 lerp(Vec2, float);

	Vec2 operator+(const Vec2&);
	Vec2 operator*(const float&);
};

struct Vec3
{
	float x;
	float y;
	float z;

	static Vec3 normal(Vec3, Vec3, Vec3);

	float mag_squared();
	float mag();
	Vec3 norm();
	float dot(Vec3);
	Vec3 cross(Vec3);
	Vec3 lerp(Vec3, float);

	Vec3 operator+(const Vec3&);
	Vec3 operator-(const Vec3&);
	Vec3 operator+=(const Vec3&);
	Vec3 operator*(const float&);
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
	static Mat4 translation(float, float, float);
	static Mat4 translation(Vec3);
	static Mat4 rotation_x(float);
	static Mat4 rotation_y(float);
	static Mat4 rotation_z(float);
	static Mat4 perspective(float, float, float, float);

	Mat4 operator*(const Mat4&);
};

#endif
