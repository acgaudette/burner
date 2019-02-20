#version 400 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 frag_pos;
out vec3 frag_normal;

uniform mat4 model;
uniform mat4 proj_view;

void main()
{
	frag_pos = (model * vec4(pos.xyz, 1)).xyz;
	frag_normal = (model * vec4(normal, 0)).xyz;
	frag_normal = normalize(frag_normal);
	gl_Position = proj_view * model * vec4(pos, 1);
}
