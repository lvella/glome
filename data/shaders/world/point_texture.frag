#version 150 core

uniform sampler2D texbase;
uniform bool has_tex;

void get_texel(inout vec4 color)
{
	if(has_tex) {
		vec4 texel = texture2D(texbase, gl_PointCoord);
		if(texel.a < 0.004)
			discard;

		color.rgb = color.rgb * texel.r;
		color.a = color.a * texel.g;
	}
}
