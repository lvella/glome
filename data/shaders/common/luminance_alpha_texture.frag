#version 150 core

in vec2 v_texcoord;
uniform sampler2D texbase;
uniform bool has_tex;

void get_texel(inout vec4 color)
{
	if(has_tex) {
		vec4 texel = texture2D(texbase, v_texcoord);
		if(texel.g < 0.004)
			discard;

		color.rgb = color.rgb * texel.r;
		color.a = color.a * texel.g;
	}
}
