#version 150 core

in vec4 v_color;

out vec4 frag_color;

void get_texel(inout vec4 color);
void mix_fog(inout vec4 color);

void main()
{
	vec4 color = v_color;

	get_texel(color);
	mix_fog(color);

	frag_color = color;
}
