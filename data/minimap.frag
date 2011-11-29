varying vec2 v_texcoord;
varying vec4 v_color;

uniform sampler2D texbase;
	
void main()
{
	vec4 tex_color = texture2D(texbase, v_texcoord);

	if(tex_color.a < 0.004)
		discard;

	gl_FragColor = v_color * tex_color;
}
