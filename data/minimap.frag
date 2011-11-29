varying vec2 v_texcoord;
varying vec4 v_color;

uniform sampler2D texbase;
uniform bool has_tex;

// TODO: put this function in a separated place
void get_texel(inout vec4 color)
{
	if(has_tex) {
		vec4 texel = texture2D(texbase, v_texcoord);
		if(texel.a < 0.004)
			discard;

		color = color * texel;
	}
}
	
void main()
{

	gl_FragColor = v_color;
	get_texel(gl_FragColor);
}

