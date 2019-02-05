#include <math.h>
#include "alg.h"

#define PI 3.14159265

float Vec2::mag_squared()
{
	return (this->x * this->x) + (this->y * this->y);
}

float Vec2::mag()
{
	return sqrt(this->mag_squared());
}

Vec2 Vec2::norm()
{
	float mag = this->mag();

	if (mag == 0) {
		return Vec2 { 0, 0 };
	}

	return Vec2 {
		this->x / mag,
		this->y / mag,
	};
}

float Vec3::mag_squared()
{
	return (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
}

float Vec3::mag()
{
	return sqrt(this->mag_squared());
}

Vec3 Vec3::norm()
{
	float mag = this->mag();

	if (mag == 0) {
		return Vec3 { 0, 0, 0 };
	}

	return Vec3 {
		this->x / mag,
		this->y / mag,
		this->z / mag,
	};
}

Vec3 Vec3::cross(Vec3 other)
{
	return Vec3 {
		this->y * other.z - this->z * other.y,
		this->z * other.x - this->x * other.z,
		this->x * other.y - this->y * other.x,
	};
}

Vec3 Vec3::normal(Vec3 a, Vec3 b, Vec3 c)
{
	return (b - a).cross(c - a);
}

Vec3 Vec3::operator+(const Vec3 &b)
{
	return Vec3 {
		this->x + b.x,
		this->y + b.y,
		this->z + b.z,
	};
}

Vec3 Vec3::operator-(const Vec3 &b)
{
	return Vec3 {
		this->x - b.x,
		this->y - b.y,
		this->z - b.z,
	};
}

Vec3 Vec3::operator+=(const Vec3 &b)
{
	this->x += b.x;
	this->y += b.y;
	this->z += b.z;

	return *this;
}

float Vec4::dot(Vec4 other)
{
	return    this->values[0] * other.values[0]
		+ this->values[1] * other.values[1]
		+ this->values[2] * other.values[2]
		+ this->values[3] * other.values[3];
}

Mat4::Mat4() {}

Mat4::Mat4(float values[16])
{
	for (size_t i = 0; i < 4; ++i) {
		for (size_t j = 0; j < 4; ++j) {
			this->values[4 * j + i] = values[4 * i + j];
		}
	}
}

Mat4 Mat4::id()
{
	float values[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	return Mat4(values);
}

Mat4 Mat4::translation(float x, float y, float z)
{
	float values[] = {
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1
	};

	return Mat4(values);
}

Mat4 Mat4::rotation_y(float rad)
{
	float values[] = {
		 cos(rad), 0, sin(rad), 0,
		        0, 1,        0, 0,
		-sin(rad), 0, cos(rad), 0,
		        0, 0,        0, 1
	};

	return Mat4(values);
}

Mat4 Mat4::perspective(float fov, float aspect, float near, float far)
{
	float half_angle = (0.5 * fov) * PI / 180;
	float y_scale = 1 / tan(half_angle);
	float x_scale = y_scale / aspect;

	float diff = far - near;
	float z_scale = (near + far) / diff;
	float z_off = (-2 * near * far) / diff;

	float values[] = {
		x_scale, 0.0, 0.0, 0.0,
		0.0, y_scale, 0.0, 0.0,
		0.0, 0.0, z_scale, z_off,
		0.0, 0.0, 1.0, 0.0,
	};

	return Mat4(values);
}

Mat4 Mat4::operator*(const Mat4 &b)
{
	Mat4 a = *this;

	// Rows
	Vec4 ax = {{ a.values[0], a.values[4], a.values[8],  a.values[12] }};
	Vec4 ay = {{ a.values[1], a.values[5], a.values[9],  a.values[13] }};
	Vec4 az = {{ a.values[2], a.values[6], a.values[10], a.values[14] }};
	Vec4 aw = {{ a.values[3], a.values[7], a.values[11], a.values[15] }};

	// Columns
	Vec4 bx = {{ b.values[0],  b.values[1],  b.values[2],  b.values[3]  }};
	Vec4 by = {{ b.values[4],  b.values[5],  b.values[6],  b.values[7]  }};
	Vec4 bz = {{ b.values[8],  b.values[9],  b.values[10], b.values[11] }};
	Vec4 bw = {{ b.values[12], b.values[13], b.values[14], b.values[15] }};

	float values[] = {
		ax.dot(bx), ax.dot(by), ax.dot(bz), ax.dot(bw),
		ay.dot(bx), ay.dot(by), ay.dot(bz), ay.dot(bw),
		az.dot(bx), az.dot(by), az.dot(bz), az.dot(bw),
		aw.dot(bx), aw.dot(by), aw.dot(bz), aw.dot(bw)
	};

	return Mat4(values);
}
