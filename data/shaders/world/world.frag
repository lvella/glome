#version 150 core

varying vec4 v_color;

void get_texel(inout vec4 color);
void mix_fog(inout vec4 color);

void main()
{
	vec4 color = v_color;

	get_texel(color);
	mix_fog(color);

	gl_FragColor = color;
}
