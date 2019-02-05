#ifndef INPUT_H
#define INPUT_H

enum Key {
	W,
	A,
	S,
	D,
	Q,
	E,
	_key_count
};

/* Thin wrapper around window library.
 * Will be extended with additional input management later if necessary.
 */
struct Input
{
	struct State {
		bool curr;
		bool last;
	};

	State keys[Key::_key_count];

	static int key_to_glfw(Key);

	bool down(Key key);
	bool up(Key key);
	bool held(Key key);
};

#endif
