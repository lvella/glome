varying vec4 v_color;

void get_texel(inout vec4 color);

void main()
{
	gl_FragColor = v_color;
	get_texel(gl_FragColor);
}

