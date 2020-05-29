#version 150 core

uniform float burn_progress;

in vec4 v_color;
noperspective in float c_dist;

out vec4 frag_color;

void mix_fog(inout vec4 color);

void main()
{
	const float burn_len = 0.001;

	float abs_dist = abs(c_dist);
	if(abs_dist > burn_progress) {
		discard;
	}

	float intact_len = burn_progress - burn_len;
	vec4 color = abs_dist > intact_len ?
		mix(
			vec4(1.0, 0.2, 0.0, 1.0),
			vec4(0.5, 0.5, 0.5, 1.0),
			(abs_dist - intact_len) / burn_len
		):
		v_color;

	mix_fog(color);
	frag_color = color;
}
