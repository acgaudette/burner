#include <math.h>
#include "alg.h"

#define PI 3.14159265

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
