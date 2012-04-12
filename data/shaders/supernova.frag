uniform vec4 center;

varying float v_color;
varying vec3 normal;
varying vec3 frag_pos;

void mix_fog(inout vec4 color);

void main()
{
	vec4 color;
	float cos_angle;

	color = vec4(v_color * 0.5, 0.0, 0.0, 1.0);
	mix_fog(color);

	cos_angle = dot(normalize(normal), -normalize(frag_pos));
	if(cos_angle < 0.30) {
		vec4 gloom;

		cos_angle = cos_angle / 0.30;
		gloom = vec4(1.0, 1.0, 0.2, cos_angle);
		color = mix(gloom, color, cos_angle);
	}

	gl_FragColor = color;
}
