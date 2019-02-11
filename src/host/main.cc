#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "game.h"

// Linux headers
#include <dlfcn.h>

#define TITLE "burner"

void window_error(int error, const char *message)
{
	fprintf(stderr, "Window error: %s\n", message);
}

int main()
{
	/* Load game function pointers */

	void *lib = dlopen("./out/libjam.so", RTLD_NOW | RTLD_LOCAL);

	if (!lib) {
		char *err = dlerror();
		fprintf(stderr, "Error loading game library: %s\n", err);
		panic();
	}

	game_start_loader *start_loader
		= (game_start_loader*)dlsym(lib, "get_start");

	if (!start_loader) {
		char *err = dlerror();
		fprintf(stderr, "Error loading start function: %s\n", err);
		panic();
	}

	game_start *start = (*start_loader)();

	game_update_loader *update_loader
		= (game_update_loader*)dlsym(lib, "get_update");

	if (!update_loader) {
		char *err = dlerror();
		fprintf(stderr, "Error loading update function: %s\n", err);
		panic();
	}

	game_update *update = (*update_loader)();

	/* Create window */

	glfwSetErrorCallback(window_error); // Window debugging

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize window\n");
		panic();
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR);

#if defined(__APPLE__)
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Use core profile only
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(
		WIDTH,
		HEIGHT,
		TITLE,
		NULL,
		NULL
	);

	if (!window) {
		fprintf(stderr, "Failed to create window\n");
		panic();
	}

	// Initialize renderer
	Renderer renderer;
	renderer.init(window);

	// Create engine state and input
	State state;
	Input input;

	// Execute game start hook
	(*start)(&state, &renderer);

	/* Main loop */

	double time;
	double last_time = 0;

	while (true) {
		if (glfwWindowShouldClose(window)) {
			break;
		}

		// Handle input
		glfwPollEvents();
		for (int i = 0; i < Key::_key_count; ++i) {
			input.keys[i].last = input.keys[i].curr;
			input.keys[i].curr = glfwGetKey(
				window,
				Input::key_to_glfw((Key)i)
			) == GLFW_PRESS;
		}

		// Time
		last_time = time;
		time = glfwGetTime();
		double delta = time - last_time;

		// Execute game update hook
		Mat4 view = (*update)(&state, &renderer, input, time, delta);

		// Render
		renderer.render(view, &state);

		glfwSwapBuffers(window); // Double buffer
	}

	/* Exit */

	// Linux
	dlclose(lib);

	glfwTerminate();
	printf("Terminated.\n");
	exit(0);
}
