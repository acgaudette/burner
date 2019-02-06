#version 400 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 frag_pos;
out vec3 frag_normal;

uniform mat4 model;
uniform mat4 proj_view;

void main()
{
	frag_pos = pos.xyz;
	frag_normal = normal;
	gl_Position = proj_view * model * vec4(pos, 1);
}
