uniform vec4 center;
uniform vec2 slerp_arc;

varying vec3 normal;
varying vec3 frag_pos;
varying vec3 direction;

void mix_fog(inout vec4 color);
float snoise(vec3 v);

void main()
{
	vec4 color;
	float cos_angle = dot(normalize(normal), -normalize(frag_pos));

	if(center.w < 0.3 && cos_angle < center.w) {
		float alpha = cos_angle / center.w;
		vec2 dir_screen = normalize(frag_pos.xy);
		color = vec4(1.0, 1.0 - dir_screen.x * dir_screen.y, 0.5, alpha*alpha*alpha*alpha);
	} else {
		float n0 = snoise(direction * slerp_arc.x * 30.0f);
		float n1 = snoise(frag_pos * 10.0f); // TODO: Use texture instead of calculating.
		color =
		vec4(
		     mix(
		         vec2((1.0 - n0) * 0.35, 0.0),
		         vec2(0.8, 0.6) * (1.0 - n1),
		         n0
		        ),
		     0.0,
		     1.0
		    );
		mix_fog(color);
	}

	gl_FragColor = color;
}
