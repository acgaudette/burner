#include <GLFW/glfw3.h>
#include "input.h"

int Input::key_to_glfw(Key key)
{
	switch (key) {
	case W: return GLFW_KEY_W;
	case A: return GLFW_KEY_A;
	case S: return GLFW_KEY_S;
	case D: return GLFW_KEY_D;
	case Q: return GLFW_KEY_Q;
	case E: return GLFW_KEY_E;
	default: return -1;
	}
}
