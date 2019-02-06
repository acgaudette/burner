#version 400 core

in  vec3 frag_normal;
out vec4 out_color;
uniform vec3 color;

void main()
{
	float t = 0.5 * frag_normal.y + 0.5;
	vec3 hemi = t * vec3(1) + (1 - t) * vec3(0);
	out_color = vec4(hemi * color, 1);
}
