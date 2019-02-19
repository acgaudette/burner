#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <GL/gl3w.h> // Runtime function loader (core only)
#include <GLFW/glfw3.h> // OpenGL included
#include "render.h"

#ifdef INLINE_GLSL
#include "glsl_core_vert.h"
#include "glsl_core_frag.h"
#endif

#define LOG_BUF 1024

void panic()
{
	glfwTerminate();
	printf("Terminated unexpectedly.\n");
	exit(1);
}

void load_gl_functions()
{
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

	size_t count = fread(buffer, 1, len, file);
	if (ferror(file)) {
		fprintf(stderr, "Error reading file\n");
		panic();
	}

	buffer[len - 1] = 0;

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

		fprintf(stderr, "\"\n%s\n\"\n", buffer);
		panic();
	}

	free(buffer);
	printf("Read %lu bytes from \"%s\"\n", count, filename);
	return shader;
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

void Renderer::init(void *window_ptr)
{
	GLFWwindow *window = (GLFWwindow*)(window_ptr);

	// Register resize callback
	glfwSetFramebufferSizeCallback(window, resize);

	// Make OpenGL context
	glfwMakeContextCurrent(window);

	// Load OpenGL function pointers
	load_gl_functions();

#if !defined(__APPLE__)
	// OpenGL debugging
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_error, 0);
#endif

	// Create rendering viewport
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);

#if defined(__APPLE__)
	{
		// GLFW's cocoa back-end does not
		// seem to initialize the window correctly
		// for high dpi (MacOS retina),
		// changing the window position and then re-setting it
		// is a work-around
		int xpos = 0;
		int ypos = 0;
		glfwGetWindowPos(window, &xpos, &ypos);
		glfwSetWindowPos(window, 0, 0);
		glfwSetWindowPos(window, xpos, ypos);
	}
#endif

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
		fprintf(stderr, "Shader %s", gl_log);
		panic();
	}

	// Uniforms
	this->color_loc = glGetUniformLocation(program, "color");
	this->model_loc = glGetUniformLocation(program, "model");
	this->proj_view_loc  = glGetUniformLocation(program, "proj_view");

	glUseProgram(program); // Single core shader
	glDeleteShader(vert);
	glDeleteShader(frag);
}

void Renderer::render(Mat4 view, State *state)
{
	// Recompute screen dimensions
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int width = viewport[2];
	int height = viewport[3];

	// Update view and projection matrices
	float aspect = (float)width / height;
	Mat4 proj = Mat4::perspective(this->fov, aspect, 0.1f, 1024);
	Mat4 proj_view = proj * view;

	glUniformMatrix4fv(
		this->proj_view_loc,
		1,
		GL_FALSE,
		proj_view.values
	);

	glClearColor(
		this->clear_color.r,
		this->clear_color.g,
		this->clear_color.b,
		1.0f
	);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render all entities
	for (size_t i = 0; i < state->ent_count; ++i) {
		size_t mesh_id = state->mesh_ids[i];
		Mesh *mesh = this->meshes[mesh_id];
		glBindVertexArray(this->objects[mesh_id]);

		Instance instance = state->ent_data[i];

		glUniform3f(
			this->color_loc,
			instance.color.r,
			instance.color.g,
			instance.color.b
		);

		glUniformMatrix4fv(
			this->model_loc,
			1,
			GL_FALSE,
			instance.model.values
		);

		glDrawElements(
			GL_TRIANGLES,
			mesh->index_count,
			GL_UNSIGNED_INT,
			0
		);
	}
}

size_t Renderer::add_mesh(Mesh *mesh, bool compute_normals)
{
	// Compute normals
	if (compute_normals) {
		for (size_t i = 0; i < mesh->index_count - 2; i += 3) {
			GLuint j = mesh->indices[i    ];
			GLuint k = mesh->indices[i + 1];
			GLuint l = mesh->indices[i + 2];

			Vec3 normal = Vec3::normal(
				mesh->vertices[j].position,
				mesh->vertices[k].position,
				mesh->vertices[l].position
			);

			mesh->vertices[j].normal += normal;
			mesh->vertices[k].normal += normal;
			mesh->vertices[l].normal += normal;
		}

		for (size_t i = 0; i < mesh->vertex_count; ++i) {
			mesh->vertices[i].normal
				= mesh->vertices[i].normal.norm();
		}
	}

	// Update mesh array
	this->meshes[this->mesh_count] = mesh;

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
		mesh->vertex_count * VERT_STRIDE * sizeof(float),
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

	/* Vertex attributes */

	// Position
	glVertexAttribPointer(
		0, // Location
		3,
		GL_FLOAT,
		GL_FALSE,
		VERT_STRIDE * sizeof(float), // Stride
		(void*)0 // Offset
	);

	glEnableVertexAttribArray(0);

	// Normal
	glVertexAttribPointer(
		1, // Location
		3,
		GL_FLOAT,
		GL_FALSE,
		VERT_STRIDE * sizeof(float), // Stride
		(void*)(3 * sizeof(float)) // Offset
	);

	glEnableVertexAttribArray(1);

	// Update object array and count
	this->objects[this->mesh_count] = vao;

	printf(
		"Added mesh %lu with %lu vertices and %lu indices\n",
		this->mesh_count,
		mesh->vertex_count,
		mesh->index_count
	);

	return this->mesh_count++;
}
