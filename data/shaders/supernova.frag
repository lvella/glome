varying float v_color;
varying vec3 normal;
varying vec3 frag_pos;

void mix_fog(inout vec4 color);

void main()
{
	vec4 color;

	if(dot(normalize(normal), -normalize(frag_pos)) < 0.1) {
		color = vec4(1.0, 1.0, 0.3, 1.0);
	}
	else {
		color = vec4(v_color * 0.5, 0.0, 0.0, 1.0);
		mix_fog(color);
	}

	gl_FragColor = color;
}
