uniform vec4 center;

varying float v_color;
varying vec3 normal;
varying vec3 frag_pos;

void mix_fog(inout vec4 color);

void main()
{
	vec4 color;
	float cos_angle = dot(normalize(normal), -normalize(frag_pos));

	if(center.w < 0.3 && cos_angle < center.w) {
		float alpha = cos_angle * 0.7 / center.w;
		color = vec4(1.0, 1.0, 0.0, alpha);
	} else {
		color = vec4(v_color * 0.5, 0.0, 0.0, 1.0);
		mix_fog(color);
	}

	gl_FragColor = color;
}
