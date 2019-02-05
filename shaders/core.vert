#version 400 core

layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 proj_view;

void main()
{
	gl_Position = proj_view * model
		* vec4(pos, 1);
}
