#version 150 core

in vec4 v_color;

out vec4 frag_color;

void get_texel(inout vec4 color);

void main()
{
	frag_color = v_color;
	get_texel(frag_color);
}
