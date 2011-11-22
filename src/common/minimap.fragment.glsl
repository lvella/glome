varying vec2 v_texcoord;
varying vec4 v_color;

uniform sampler2D texbase;
	
void main()
{
	vec4 tex_color = texture2D(texbase, v_texcoord);

	gl_FragColor.xyz = v_color.xyz;
	gl_FragColor.w = v_color.w * tex_color.w;
}