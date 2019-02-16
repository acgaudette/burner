#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "burner.h"

// Linux headers
#include <dlfcn.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#define TITLE "burner"

void window_error(int error, const char *message)
{
	fprintf(stderr, "Window error: %s\n", message);
}

struct GameLibrary {
	void *lib;
	game_start *start;
	game_update *update;

	time_t prev_load_time;
};

bool load_game(GameLibrary *game)
{
	return false;
}
bool unload_game(GameLibrary *game)
{
	return false;
}


int Engine_init(void* custom_data)
{
	/* Load game function pointers */

	GameLibrary game;

	void* user_data = custom_data;

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

	game.lib    = lib;
	game.start  = start;
	game.update = update;

	struct stat file_info;
	if (stat("./out/libjam.so", &file_info)) {
		fprintf(stderr, "Error getting library modification time: %s\n", strerror(errno));
		panic();
	}
	game.prev_load_time = file_info.st_mtime;

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
	(*game.start)(&state, &renderer, &user_data);

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
		Mat4 view = (*game.update)(&state, &renderer, &input, time, delta, user_data);

		// Render
		renderer.render(view, &state);

		glfwSwapBuffers(window); // Double buffer
	}

	/* Exit */

	// Linux
	dlclose(game.lib);

	glfwTerminate();
	printf("Terminated.\n");
	return EXIT_SUCCESS;	
}
