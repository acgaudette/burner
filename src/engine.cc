#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <GL/gl3w.h> // Runtime function loader (core only)
#include <GLFW/glfw3.h> // OpenGL included
#include "engine.h"

#define WIDTH  1280
#define HEIGHT 720
#define TITLE "burner"
#define CLEAR 0.2f, 0.1f, 0.1f
#define MAJOR 4
#define MINOR 0
#define LOG_BUF 1024

void panic()
{
	glfwTerminate();
	printf("Terminated unexpectedly.\n");
	exit(1);
}

GLuint load_shader(const char *filename, GLenum type, char *log)
{
	/* Read file into string */

	FILE *file = fopen(filename, "r");

	if (!file) {
		perror("Error");
		panic();
	}

	if (fseek(file, 0, SEEK_END) == -1) {
		perror("Error");
		panic();
	}

	errno = 0;
	long len = ftell(file);
	if (errno) {
		perror("Error");
		panic();
	}

	// Rewind
	if (fseek(file, 0, SEEK_SET) == -1) {
		perror("Error");
		panic();
	}

	char *buffer = (char*)malloc(len);

	if (!buffer) {
		perror("Error");
		panic();
	}

	fread(buffer, 1, len, file);
	if (ferror(file)) {
		fprintf(stderr, "Error reading file\n");
		panic();
	}

	if (fclose(file)) {
		perror("Error");
		panic();
	}

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &buffer, NULL);
	glCompileShader(shader);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(shader, LOG_BUF, NULL, log);

		fprintf(
			stderr,
			"Compilation error in \"%s\":\n%s",
			filename,
			log
		);

		panic();
	}

	return shader;
}

void window_error(int error, const char *message)
{
	fprintf(stderr, "Window error: %s\n", message);
}

void gl_error(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void*
) {
	fprintf(
		stderr,
		"OpenGL Error: %s\n",
		message
	);
}

void resize(GLFWwindow *window, int width, int height)
{
	// Resize OpenGL viewport
	glViewport(0, 0, width, height);
	printf("Window resize: %dx%d\n", width, height);
}

Engine::Engine(): count(0), width(WIDTH), height(HEIGHT), view(Mat4::id())
{
	for (size_t i = 0; i < MAX_MESH; ++i) {
		this->meshes[i] = nullptr;
	}
}

void Engine::init(Game &game)
{
	glfwSetErrorCallback(window_error);

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize window\n");
		panic();
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR);

	// Use core profile only
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create window */

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

	// Register resize callback
	glfwSetFramebufferSizeCallback(window, resize);

	// Make OpenGL context
	glfwMakeContextCurrent(window);

	// Load functions at runtime
	if (gl3wInit()) {
		fprintf(stderr, "Failed to initialize loader\n");
		panic();
	}

	// Check for compatability
	if (!gl3wIsSupported(MAJOR, MINOR)) {
		fprintf(
			stderr,
			"OpenGL %d.%d is not supported\n",
			MAJOR,
			MINOR
		);

		panic();
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_error, 0);

	// Create rendering viewport
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(CLEAR, 1.0f);

	/* Shaders */

	char gl_log[LOG_BUF];

	GLuint vert = load_shader(
		"shaders/core.vert",
		GL_VERTEX_SHADER,
		gl_log
	);

	GLuint frag = load_shader(
		"shaders/core.frag",
		GL_FRAGMENT_SHADER,
		gl_log
	);

	GLuint program;
	program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);

	int result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(program, LOG_BUF, NULL, gl_log);
		panic();
	}

	// Uniforms
	GLint color_loc = glGetUniformLocation(program, "color");

	glUseProgram(program); // Single core shader
	glDeleteShader(vert);
	glDeleteShader(frag);

	// Execute game start hook
	game.start();

	/* Main loop */

	while (true) {
		if (glfwWindowShouldClose(window)) {
			break;
		}

		// Input handling
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		// Execute game update hook
		game.update();

		/* Rendering */

		glClear(GL_COLOR_BUFFER_BIT);

		// Render all meshes
		for (size_t i = 0; i < this->count; ++i) {
			Mesh *mesh = this->meshes[i];
			glBindVertexArray(this->objects[i]);

			glUniform3f(
				color_loc,
				mesh->color.r,
				mesh->color.g,
				mesh->color.b
			);

			glDrawElements(
				GL_TRIANGLES,
				mesh->index_count,
				GL_UNSIGNED_INT,
				0
			);
		}

		glfwSwapBuffers(window); // Double buffer
	}

	// Exit
	glfwTerminate();
	printf("Terminated.\n");
	exit(0);
}

void Engine::add_mesh(Mesh *mesh)
{
	// Update mesh array
	this->meshes[this->count] = mesh;

	// Vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Vertex buffer
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		// FIXME: const
		3 * mesh->vertex_count * sizeof(float),
		mesh->vertices,
		GL_STATIC_DRAW
	);

	// Index buffer
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		mesh->index_count * sizeof(unsigned int),
		mesh->indices,
		GL_STATIC_DRAW
	);

	// Vertex attributes
	glVertexAttribPointer(
		0, // Location
		mesh->vertex_count,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(float), // Stride
		(void*)0 // Offset
	);

	glEnableVertexAttribArray(0);

	// Update object array and count
	this->objects[this->count] = vao;
	this->count++;

	printf(
		"Added mesh %lu with %lu vertices and %lu indices\n",
		this->count,
		mesh->vertex_count,
		mesh->index_count
	);
}
