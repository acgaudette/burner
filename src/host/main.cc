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

#if defined(DEVELOPMENT_MODE)
    #warning "DEV"
#else
    #warning "NON-DEV"
#endif

void window_error(int error, const char *message)
{
	fprintf(stderr, "Window error: %s\n", message);
}

struct FileConfig {
	const char * src_path;
	const char * lib_name;
	const char * shaders_path;
};

struct GameLibrary {
	void *lib;
	game_start *start;
	game_update *update;
	game_on_reload *on_reload;

	time_t prev_load_time;

	FileConfig conf;
};

bool file_modification_time_try_to_update(const char * const file_path, time_t *time_to_update)
{
	struct stat file_info;
	if (stat(file_path, &file_info)) {
		fprintf(stderr, "Error getting [%s] source modification time: %s\n", 
			file_path, strerror(errno));
		return false;
	}
	if (file_info.st_mtime > *time_to_update) {
		*time_to_update = file_info.st_mtime;
		return true;
	}
	
	return false;
}

static const char * const swap_names[2] = {
	"./out/libjam.so",
	"./out/libjam_swap.so"	
};
static size_t swap_name_idx = 0;
static size_t lib_version = 0;

bool GameLibrary_unload(GameLibrary& game)
{
	if (dlclose(game.lib) != 0) {
		fprintf(stderr, "Error: library could not close");
		panic();
	}

	game.lib = nullptr;

	return true;
}

bool GameLibrary_recompile(const char * const script_name, const char * const src_name, const char * const lib_name)
{	
	char cmd[128];

	strcpy(cmd, script_name);
	strcat(cmd, " ");
	strcat(cmd, src_name);
	strcat(cmd, " ");
	strcat(cmd, lib_name);

	puts(cmd);

	return (system(cmd) == 0);
}

bool GameLibrary_load(GameLibrary& game, 
	const char * const lib_name = "./out/libjam.so",
	const char * const src_name = "examples/simple.cc",
	const char * const version = "0"
);
bool GameLibrary_load(GameLibrary& game,
	const char * const lib_name,
	const char * const src_name,
	const char * const version
)
{
	printf("loading %s\n", lib_name);
	void *lib = dlopen(lib_name, RTLD_LAZY);

	if (!lib) {
		char *err = dlerror();
		fprintf(stderr, "Error loading game library: %s\n", err);
		panic();
	}

	char sym_name[32];
	{
		strcpy(sym_name, "get_start");
		strcat(sym_name, version);
	}
	game_start_loader *start_loader
		= (game_start_loader*)dlsym(lib, sym_name);

	if (!start_loader) {
		char *err = dlerror();
		fprintf(stderr, "Error loading start function: %s\n", err);
		panic();
	}
	game_start *start = (*start_loader)();


	{
		strcpy(sym_name, "get_update");
		strcat(sym_name, version);
	}
	game_update_loader *update_loader
		= (game_update_loader*)dlsym(lib, sym_name);

	if (!update_loader) {
		char *err = dlerror();
		fprintf(stderr, "Error loading update function: %s\n", err);
		panic();
	}
	game_update *update = (*update_loader)();

	{
		strcpy(sym_name, "get_on_reload");
		strcat(sym_name, version);
	}
	game_on_reload_loader *on_reload_loader
		= (game_on_reload_loader*)dlsym(lib, sym_name);

	game_on_reload *on_reload = nullptr;
	if (!on_reload_loader) {
		char *err = dlerror();
		if (err != nullptr) {
			fprintf(stderr, "Error loading on_reload function: %s\n", err);
			return false;
		}
	} else {
		 on_reload = (*on_reload_loader)();
	}

	file_modification_time_try_to_update(src_name, &game.prev_load_time);

	if (game.lib != nullptr) {
		GameLibrary_unload(game);
	}

	game.lib       = lib;
	game.start     = start;
	game.update    = update;
	game.on_reload = on_reload;

	swap_name_idx = 1 - swap_name_idx;
	lib_version += 1;

	return true;
}

bool GameLibrary_reload(GameLibrary& game, const char * const src_name = "examples/simple.cc");
bool GameLibrary_reload(GameLibrary& game, const char * const src_name)
{
	char file_contents[32];
	char version[32];
	sprintf(version, "%zu", lib_version);

	{
		strcpy(file_contents, "#define LIB_VERSION ");
		strcat(file_contents, version);

		printf("File contents: %s\n", file_contents);

		FILE *fptr = fopen("out/version.h", "w");
		if (!fptr) {
			fprintf(stderr, "Error: failed to open version file");
			return false;
		}
		fwrite(file_contents, sizeof(char), strlen(file_contents), fptr);
		fflush(fptr);
		fclose(fptr);
	}

	const char * const lib_name = swap_names[swap_name_idx];
	printf("compiling %s to build %s\n", src_name, lib_name);
	bool success = GameLibrary_recompile("./live_build_macos", src_name, lib_name);
	if (!success) {
		fprintf(stderr, "Error: could not compile dynamic library\n");
		return false;
	}

	return GameLibrary_load(game, lib_name, src_name, version);
}


static bool poll_src_changes = false;
void window_focus_callback(GLFWwindow* window, int focused)
{
    poll_src_changes = !focused;
}


int Engine::run(const char * const src_name, void* custom_data, size_t custom_data_size)
{
	/* Load game function pointers */

	GameLibrary game;
	memset(&game, 0, sizeof(game));
	GameLibrary_load(game);

	void* user_data = custom_data;
	size_t user_data_size = custom_data_size;

	struct SaveStates {
		void* data;
		Input input_states[10];
		double time_states[10];
		double last_time_states[10];
	} saves;
	memset(&saves, 0, sizeof(SaveStates));

	if (user_data != nullptr) { 
		saves.data = calloc(10, custom_data_size); // just one save for now, basic testing
		if (saves.data == nullptr) {
			fprintf(stderr, "Error memory allocation, aborting");
			panic();
		}
	}


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

	glfwSetWindowFocusCallback(window, window_focus_callback);

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
	double time_offset = 0;

	while (true) {
		if (glfwWindowShouldClose(window)) {
			break;
		}

		// Reload main project if in development mode


		// Handle input
		glfwPollEvents();
		for (int i = 0; i < Key::_key_count; ++i) {
			input.keys[i].last = input.keys[i].curr;
			input.keys[i].curr = glfwGetKey(
				window,
				Input::key_to_glfw((Key)i)
			) == GLFW_PRESS;
		}

#if defined(DEVELOPMENT_MODE)
		if (poll_src_changes || input.down(GRAVE_ACCENT)) {
			if (file_modification_time_try_to_update(src_name, &game.prev_load_time)) {
				puts("reloading library");
				if (GameLibrary_reload(game) && game.on_reload != nullptr) {
					game.on_reload(&state, &renderer, &user_data);
				}
			}
		}

		// early save states testing, not 100% sure whether I covered all cases for time "travel"
		if (input.down(SLASH)) {

			#define ANSI_COLOR_RED     "\x1b[31m"
			#define ANSI_COLOR_GREEN   "\x1b[32m"
			#define ANSI_COLOR_YELLOW  "\x1b[33m"
			#define ANSI_COLOR_BLUE    "\x1b[34m"
			#define ANSI_COLOR_MAGENTA "\x1b[35m"
			#define ANSI_COLOR_CYAN    "\x1b[36m"
			#define ANSI_COLOR_RESET   "\x1b[0m"

			memcpy(saves.data, (char*)user_data, user_data_size);
			saves.input_states[0] = input;
			saves.time_states[0] = time;

			printf(ANSI_COLOR_GREEN "State saved, [slot=%d, time=%f]" ANSI_COLOR_RESET "\n", 
				0, time);

		} else if (input.down(PERIOD)) {
			memcpy(user_data, saves.data, user_data_size);
			time_offset = glfwGetTime() - saves.time_states[0]; // TODO could be a little inaccurate?

			printf(ANSI_COLOR_GREEN "State restored, [slot=%d, time=%f]" ANSI_COLOR_RESET "\n", 
				0, saves.time_states[0]);
		}
#endif





		// Time
		last_time = time;
		time = glfwGetTime() - time_offset;
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

	free(saves.data);

	glfwTerminate();
	printf("Terminated.\n");
	return EXIT_SUCCESS;	
}
