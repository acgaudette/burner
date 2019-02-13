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
	case R: return GLFW_KEY_R;
	case H: return GLFW_KEY_H;
	case J: return GLFW_KEY_J;
	case K: return GLFW_KEY_K;
	case L: return GLFW_KEY_L;
	case L_BRACK: return GLFW_KEY_LEFT_BRACKET;
	case R_BRACK: return GLFW_KEY_RIGHT_BRACKET;
	case MINUS: return GLFW_KEY_MINUS;
	case EQUAL: return GLFW_KEY_EQUAL;
	case BACKSLASH: return GLFW_KEY_BACKSLASH;
	case SPACE: return GLFW_KEY_SPACE;
	default: return -1;
	}
}

bool Input::down(Key key)
{
	State state = this->keys[key];
	return state.curr && !state.last;
}

bool Input::up(Key key)
{
	State state = this->keys[key];
	return !state.curr && state.last;
}

bool Input::held(Key key)
{
	State state = this->keys[key];
	return state.curr && state.last;
}
