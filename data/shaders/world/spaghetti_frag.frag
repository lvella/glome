#version 150 core

uniform float burn_progress;

in vec4 v_color;
noperspective in float c_dist;

out vec4 frag_color;

void mix_fog(inout vec4 color);

// Noise code, taken from:
// https://stackoverflow.com/a/4275343/578749
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// Colors of the burning stage:
// 0 - black
// 0.1 - black
// 0.2 - orange
// 0.4 - yellow
// 0.6 - white
// 1.0 - ashes
const vec3 black = vec3(0.0, 0.0, 0.0);
const vec3 orange = vec3(0.937, 0.38, 0.098);
const vec3 yellow = vec3(0.878, 0.867, 0.016);
const vec3 white = vec3(1.0, 1.0, 1.0);
const vec3 ashes = vec3(0.84, 0.82, 0.82);

vec3 burn_color(float t)
{
	if(t < 0.1) {
		return black;
	}

	if(t < 0.2) {
		return mix(black, orange, (t - 0.1) / 0.1);
	}

	if(t < 0.4) {
		return mix(orange, yellow, (t - 0.2) / 0.2);
	}

	if(t < 0.6) {
		return mix(yellow, white, (t - 0.4) / 0.2);
	}

	return mix(white, ashes, (t - 0.6) / 0.4);
}

void main()
{
	// Same value as in spaghetti_fragment.cpp:
	const float BURN_LEN = 0.004;

	float abs_dist = abs(c_dist);
	if(abs_dist > burn_progress) {
		discard;
	}

	vec4 color;
	color.a = 1.0;

	float ashes_start = burn_progress - 0.003;
	if(abs_dist > ashes_start) {
		if(rand(vec2(abs_dist, v_color.r)) < (abs_dist - ashes_start) / 0.003) {
			discard;
		}
		// Ashes section
		color.rgb = ashes;
	} else {
		float burning_start = burn_progress - 0.004;
		if(abs_dist >= burning_start) {
			color.rgb = burn_color((abs_dist - burning_start) / 0.001);
		} else {
			color = v_color;
		}
	}

	mix_fog(color);
	frag_color = color;
}
