#version 400 core

in  vec3 frag_normal;
out vec4 out_color;
uniform vec3 color;

#define TOP vec3(1)
#define BOT vec3(0)

void main()
{
	float t = 0.5 * frag_normal.y + 0.5;
	vec3 hemi = t * TOP + (1 - t) * BOT;
	out_color = vec4(hemi * color, 1);
}
