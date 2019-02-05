#include "alg.h"

Mat4::Mat4() {}

Mat4::Mat4(float values[16])
{
	for (size_t i = 0; i < 4; ++i) {
		for (size_t j = 0; j < 4; ++j) {
			this->values[4 * j + i] = values[4 * i + j];
		}
	}
}
