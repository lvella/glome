varying vec2 v_texcoord;
uniform sampler2D texbase;
uniform bool has_tex;

void get_texel(inout vec4 color)
{
	if(has_tex) {
		vec4 texel = texture2D(texbase, v_texcoord);
		if(texel.a < 0.004)
			discard;

		color = color * texel;
	}
}