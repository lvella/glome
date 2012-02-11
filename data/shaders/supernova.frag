varying float v_color;
varying vec3 normal;
varying vec3 frag_pos;

void mix_fog(inout vec4 color);

void main()
{
	vec4 color;

	color = dot(normalize(normal), -normalize(frag_pos)) < 0.01 ?
		vec4(1.0, 1.0, 0.3, 1.0) :
		vec4(v_color * 0.5, 0.0, 0.0, 1.0);
	mix_fog(color);
	gl_FragColor = color;
}
