#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <GL/gl3w.h> // Runtime function loader (core only)
#include <GLFW/glfw3.h> // OpenGL included

#define WIDTH  1280
#define HEIGHT 720
#define TITLE "burner"
#define CLEAR 0.2f, 0.1f, 0.1f
#define MAJOR 4
#define MINOR 0
#define LOG_BUF 1024
