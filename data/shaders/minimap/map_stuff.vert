#version 150 core

uniform vec4 transformation[2];

vec4 quat_rotate(in vec4 rotation[2], in vec4 p);

vec3 project_vertex(vec4 vert) {
	const float two_by_pi = 0.63661977236758134307553505349005744813783858296183;
	vec3 ret;
	float r, R;

	vert = quat_rotate(transformation, vert);

	ret = vert.xyz / (1.0 - vert.w);
	R = length(ret);
	r = atan(R) * two_by_pi;
	ret = ret * (r / R);

	return ret;
}

vec4 color_blend(vec4 color, float z) {
	const vec4 pale_color = vec4(0.25, 0.65, 0.25, 1.0);
	return mix(pale_color, color, (z + 1.0) * 0.5);
}
